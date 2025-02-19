#include "pch.h"
#include "Core/Mesh.h"

#include "Core/Edge.h"
#include "Math/Ray3.h"
#include "Utils/FileUtils.h"

/*static*/ std::optional<Mesh> Mesh::LoadFromFile(const fs::path& filepath)
{
	const auto fileContents = utils::ReadFile(filepath);
	if (!fileContents)
	{
		LOG_ERROR("\"{}\" does not exist!", filepath.string());
		return {};
	}

	const auto hasInvalidFormat = [&filepath](const bool condition) -> bool
		{
			if (!condition)
			{
				LOG_ERROR("\"{}\" has invalid format!", filepath.string());
			}
			
			return !condition;
		};

	json::Document jsonDocument;
	jsonDocument.Parse(fileContents->c_str());
	if (hasInvalidFormat(jsonDocument.IsObject() &&
		jsonDocument.HasMember("geometry_object"))) return {};

	const auto& jsonGeometryObject = jsonDocument["geometry_object"];
	if (hasInvalidFormat(
		jsonGeometryObject.IsObject() &&
		jsonGeometryObject.HasMember("vertices") &&
		jsonGeometryObject.HasMember("triangles"))) return {};

	std::vector<Vector3f> vertices;
	{
		const auto& jsonVertices = jsonGeometryObject["vertices"];
		if (hasInvalidFormat(jsonVertices.IsArray() && jsonVertices.Size() % 3 == 0)) return {};

		vertices.reserve(jsonVertices.Size() / 3);
		for (json::SizeType i = 0; i < jsonVertices.Size(); i += 3)
		{
			const auto& jsonVertex1 = jsonVertices[i];
			const auto& jsonVertex2 = jsonVertices[i + 1];
			const auto& jsonVertex3 = jsonVertices[i + 2];

			// Could also ckeck with IsFloat, but it adds unnecessary overhead
			if (hasInvalidFormat(jsonVertex1.IsDouble() && jsonVertex2.IsDouble() && jsonVertex3.IsDouble())) return {};
			vertices.emplace_back(jsonVertex1.GetFloat(), jsonVertex2.GetFloat(), jsonVertex3.GetFloat());
		}
	}

	std::vector<Triangle> triangles;
	{
		const auto& jsonTriangles = jsonGeometryObject["triangles"];
		if (hasInvalidFormat(jsonTriangles.IsArray() && jsonTriangles.Size() % 3 == 0)) return {};

		triangles.reserve(jsonTriangles.Size() / 3);
		for (json::SizeType i = 0; i < jsonTriangles.Size(); i += 3)
		{
			const auto& jsonTriangle1 = jsonTriangles[i];
			const auto& jsonTriangle2 = jsonTriangles[i + 1];
			const auto& jsonTriangle3 = jsonTriangles[i + 2];

			if (hasInvalidFormat(jsonTriangle1.IsUint() && jsonTriangle2.IsUint() && jsonTriangle3.IsUint())) return {};
			triangles.emplace_back(jsonTriangle1.GetUint(), jsonTriangle2.GetUint(), jsonTriangle3.GetUint());
		}
	}

	return Mesh(std::move(vertices), std::move(triangles));
}

/*static*/ bool Mesh::SaveToFile(const fs::path& filepath, const Mesh& mesh)
{
	json::Document jsonDocument;
	auto& jsonAllocator = jsonDocument.GetAllocator();

	json::Value jsonVertices(json::kArrayType);
	for (const auto& vertex : mesh.m_Vertices)
	{
		jsonVertices.PushBack(vertex.x, jsonAllocator);
		jsonVertices.PushBack(vertex.y, jsonAllocator);
		jsonVertices.PushBack(vertex.z, jsonAllocator);
	}

	json::Value jsonTriangles(json::kArrayType);
	for (const auto& trianlge : mesh.m_Triangles)
	{
		jsonTriangles.PushBack(trianlge.VertexIndexes[0], jsonAllocator);
		jsonTriangles.PushBack(trianlge.VertexIndexes[1], jsonAllocator);
		jsonTriangles.PushBack(trianlge.VertexIndexes[2], jsonAllocator);
	}

	json::Value jsonGeometryObject(json::kObjectType);
	jsonGeometryObject.AddMember("vertices", jsonVertices, jsonAllocator);
	jsonGeometryObject.AddMember("triangles", jsonTriangles, jsonAllocator);

	jsonDocument.SetObject();
	jsonDocument.AddMember("geometry_object", jsonGeometryObject, jsonAllocator);

	json::StringBuffer jsonStringBuffer;
	json::Writer<json::StringBuffer> jsonWriter(jsonStringBuffer);
	jsonDocument.Accept(jsonWriter);

	return utils::WriteFile(filepath, jsonStringBuffer.GetString());
}

Mesh::Mesh(const std::vector<Vector3f>& vertices, const std::vector<Triangle>& triangles)
	: m_Vertices(vertices)
	, m_Triangles(triangles)
	, m_SmoothVertexNormals(m_Vertices.size())
{
	Init();
}

Mesh::Mesh(std::vector<Vector3f>&& vertices, std::vector<Triangle>&& triangles)
	: m_Vertices(std::move(vertices))
	, m_Triangles(std::move(triangles))
	, m_SmoothVertexNormals(m_Vertices.size())
{
	Init();
}

void Mesh::Init()
{
	ASSERT(!m_Vertices.empty() && !m_Triangles.empty());

	CalculateSmoothVertexNormals();
	CalculateStatistics();
	CalculateEdgeCountAndIsClosed();

	LOG_INFO("Vertices: {}", m_Vertices.size());
	LOG_INFO("Triangles: {}", m_Triangles.size());
	LOG_INFO("Smooth vertex normals: {}", m_SmoothVertexNormals.size());
	LOG_INFO("Smallest triangle area: {}", m_Statistics.SmallestTriangleArea);
	LOG_INFO("Biggest triangle area: {}", m_Statistics.BiggestTriangleArea);
	LOG_INFO("Average triangle area: {}", m_Statistics.AverageTriangleArea);
	LOG_INFO("Edges: {}", m_EdgeCount);
	LOG_INFO("IsClosed: {}", m_IsClosed ? "true" : "false");
}

void Mesh::CalculateSmoothVertexNormals()
{
	for (const auto& triangle : m_Triangles)
	{
		const uint32_t vertexIndex0 = triangle.VertexIndexes[0];
		const uint32_t vertexIndex1 = triangle.VertexIndexes[1];
		const uint32_t vertexIndex2 = triangle.VertexIndexes[2];

		const auto& vertex0 = m_Vertices[vertexIndex0];
		const auto& vertex1 = m_Vertices[vertexIndex1];
		const auto& vertex2 = m_Vertices[vertexIndex2];

		const auto edge1 = vertex1 - vertex0;
		const auto edge2 = vertex2 - vertex0;
		const auto normal = edge1.CrossProduct(edge2);

		m_SmoothVertexNormals[vertexIndex0] += normal;
		m_SmoothVertexNormals[vertexIndex1] += normal;
		m_SmoothVertexNormals[vertexIndex2] += normal;
	}

	for (auto& smoothVertexNormal : m_SmoothVertexNormals)
	{
		if (smoothVertexNormal.MagnitudeSquared() > EPSILON)
			smoothVertexNormal = smoothVertexNormal.Normalized();
	}
}

void Mesh::CalculateStatistics()
{
	const uint32_t triangleCount = static_cast<uint32_t>(m_Triangles.size());
	ASSERT(triangleCount > 0);

	const uint32_t hardwareConcurrency = std::thread::hardware_concurrency();
	const uint32_t availableThreadsCount = hardwareConcurrency > 0 ? hardwareConcurrency : 4;
	const uint32_t usedThreadsCount = std::min(triangleCount, availableThreadsCount);

	const uint32_t chunkSize = triangleCount / usedThreadsCount;
	uint32_t leftoverTriangles = triangleCount % usedThreadsCount;

	std::mutex vertexMtx, statsMtx;

	const auto calculateTriangleArea = [this, &vertexMtx, &statsMtx](const size_t startIndex, const size_t count) -> void
		{
			const size_t endIndex = startIndex + count;

			for (size_t i = startIndex; i < endIndex; ++i)
			{
				const auto& triangle = m_Triangles[i];

				const uint32_t vertexIndex0 = triangle.VertexIndexes[0];
				const uint32_t vertexIndex1 = triangle.VertexIndexes[1];
				const uint32_t vertexIndex2 = triangle.VertexIndexes[2];

				// Copy the vertices to evade possible race conditions
				Vector3f vertex0, vertex1, vertex2;
				{
					std::lock_guard lock(vertexMtx);

					vertex0 = m_Vertices[vertexIndex0];
					vertex1 = m_Vertices[vertexIndex1];
					vertex2 = m_Vertices[vertexIndex2];
				}

				const auto edge1 = vertex1 - vertex0;
				const auto edge2 = vertex2 - vertex0;
				const auto area = edge1.CrossProduct(edge2).Magnitude() / 2.f;

				{
					std::lock_guard lock(statsMtx);

					if (area > EPSILON && (area < m_Statistics.SmallestTriangleArea || m_Statistics.SmallestTriangleArea == 0.f))
						m_Statistics.SmallestTriangleArea = area;

					if (m_Statistics.BiggestTriangleArea < area)
						m_Statistics.BiggestTriangleArea = area;

					m_Statistics.AverageTriangleArea += area;
				}
			}
		};

	std::vector<std::thread> threads;
	threads.reserve(usedThreadsCount);
	for (size_t i = 0; i < usedThreadsCount; ++i)
	{
		size_t count = chunkSize;
		if (leftoverTriangles > 0)
		{
			++count;
			--leftoverTriangles;
		}

		threads.emplace_back(calculateTriangleArea, i * chunkSize, count);
	}

	for (auto& thread : threads)
		thread.join();

	m_Statistics.AverageTriangleArea /= triangleCount;
}

void Mesh::CalculateEdgeCountAndIsClosed()
{
	// Bucket estimate given using Euler's polyhedron formula: V - E + F = 2
	std::unordered_map<Edge, uint32_t> edgeToNeighbourCount(m_Vertices.size() + m_Triangles.size() - 2);

	for (const auto& triangle : m_Triangles)
	{
		const uint32_t vertexIndex0 = triangle.VertexIndexes[0];
		const uint32_t vertexIndex1 = triangle.VertexIndexes[1];
		const uint32_t vertexIndex2 = triangle.VertexIndexes[2];

		const Edge edge0 = { vertexIndex0, vertexIndex1 };
		const Edge edge1 = { vertexIndex1, vertexIndex2 };
		const Edge edge2 = { vertexIndex2, vertexIndex0 };

		++edgeToNeighbourCount[edge0];
		++edgeToNeighbourCount[edge1];
		++edgeToNeighbourCount[edge2];
	}

	m_EdgeCount = static_cast<uint32_t>(edgeToNeighbourCount.size());

	for (const auto& [edge, neighbourCount] : edgeToNeighbourCount)
	{
		if (neighbourCount < 2)
		{
			m_IsClosed = false;
			return;
		}
	}

	m_IsClosed = true;
}

const std::vector<Vector3f>& Mesh::GetVertices() const
{
	return m_Vertices;
}

const std::vector<Triangle>& Mesh::GetTriangles() const
{
	return m_Triangles;
}

const std::vector<Vector3f>& Mesh::GetSmoothVertexNormals() const
{
	return m_SmoothVertexNormals;
}

const Mesh::Statistics& Mesh::GetStatistics() const
{
	return m_Statistics;
}

uint32_t Mesh::GetEdgeCount() const
{
	return m_EdgeCount;
}

bool Mesh::IsClosed() const
{
	return m_IsClosed;
}

Mesh Mesh::GenerateSubdividedMesh() const
{
	std::vector<Vector3f> newVertices(m_Vertices);
	newVertices.reserve(newVertices.size() + m_EdgeCount);

	std::unordered_map<Edge, uint32_t> edgeToMidpointIndex(m_EdgeCount);

	const auto getMidpointIndex = [&](const Edge& edge) -> uint32_t
		{
			const auto it = edgeToMidpointIndex.find(edge);
			if (it != edgeToMidpointIndex.end())
				return it->second;

			auto midpoint = (m_Vertices[edge.VertexIndexes.first] + m_Vertices[edge.VertexIndexes.second]) / 2.f;
			const uint32_t midpointIndex = static_cast<uint32_t>(newVertices.size());

			newVertices.push_back(std::move(midpoint));
			edgeToMidpointIndex[edge] = midpointIndex;

			return midpointIndex;
		};

	std::vector<Triangle> newTriangles;
	newTriangles.reserve(m_Triangles.size() * 4);

	for (const auto& triangle : m_Triangles)
	{
		const uint32_t vertexIndex0 = triangle.VertexIndexes[0];
		const uint32_t vertexIndex1 = triangle.VertexIndexes[1];
		const uint32_t vertexIndex2 = triangle.VertexIndexes[2];

		const uint32_t midpointIndex0 = getMidpointIndex({ vertexIndex0, vertexIndex1 });
		const uint32_t midpointIndex1 = getMidpointIndex({ vertexIndex1, vertexIndex2 });
		const uint32_t midpointIndex2 = getMidpointIndex({ vertexIndex2, vertexIndex0 });

		newTriangles.emplace_back(vertexIndex0, midpointIndex0, midpointIndex2);
		newTriangles.emplace_back(vertexIndex1, midpointIndex1, midpointIndex0);
		newTriangles.emplace_back(vertexIndex2, midpointIndex2, midpointIndex1);
		newTriangles.emplace_back(midpointIndex0, midpointIndex1, midpointIndex2);
	}

	return Mesh(std::move(newVertices), std::move(newTriangles));
}

bool Mesh::IsPointInsideMesh(const Vector3f& point) const
{
	// Can be any direction
	static constexpr Vector3f RAY_DIRECTION = { 1.f, 0.f, 0.f };

	const Ray3f ray(point, RAY_DIRECTION);
	uint32_t intersectionCount = 0;

	for (const auto& triangle : m_Triangles)
	{
		const uint32_t vertexIndex0 = triangle.VertexIndexes[0];
		const uint32_t vertexIndex1 = triangle.VertexIndexes[1];
		const uint32_t vertexIndex2 = triangle.VertexIndexes[2];

		const auto& vertex0 = m_Vertices[vertexIndex0];
		const auto& vertex1 = m_Vertices[vertexIndex1];
		const auto& vertex2 = m_Vertices[vertexIndex2];

		if (ray.IntersectsTriangle(vertex0, vertex1, vertex2))
			++intersectionCount;
	}

	return IsOdd(intersectionCount);
}