#pragma once

#include "Core/Triangle.h"
#include "Math/Vector3.h"

class Mesh
{
public:
	struct Statistics
	{
		float SmallestTriangleArea = 0.f;
		float BiggestTriangleArea = 0.f;
		float AverageTriangleArea = 0.f;
	};

public:
	static std::optional<Mesh> LoadFromFile(const fs::path& filepath);
	static bool SaveToFile(const fs::path& filepath, const Mesh& mesh);

public:
	Mesh(const std::vector<Vector3f>& vertices, const std::vector<Triangle>& triangles);
	Mesh(std::vector<Vector3f>&& vertices, std::vector<Triangle>&& triangles);

	const std::vector<Vector3f>& GetVertices() const;
	const std::vector<Triangle>& GetTriangles() const;

	const std::vector<Vector3f>& GetSmoothVertexNormals() const;
	const Mesh::Statistics& GetStatistics() const;
	uint32_t GetEdgeCount() const;
	bool IsClosed() const;

	Mesh GenerateSubdividedMesh() const;

	bool IsPointInsideMesh(const Vector3f& point) const;

private:
	void Init();

	void CalculateSmoothVertexNormals();
	void CalculateStatistics();
	void CalculateEdgeCountAndIsClosed();

private:
	std::vector<Vector3f> m_Vertices;
	std::vector<Triangle> m_Triangles;

	std::vector<Vector3f> m_SmoothVertexNormals;
	Mesh::Statistics m_Statistics;
	uint32_t m_EdgeCount;
	bool m_IsClosed;
};