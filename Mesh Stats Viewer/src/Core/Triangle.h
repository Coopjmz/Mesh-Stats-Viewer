#pragma once

struct Triangle
{
	std::array<uint32_t, 3> VertexIndexes;

	Triangle(const uint32_t vertexIndex0, const uint32_t vertexIndex1, const uint32_t vertexIndex2);
};