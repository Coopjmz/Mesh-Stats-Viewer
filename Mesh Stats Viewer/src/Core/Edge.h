#pragma once

struct Edge
{
	std::pair<uint32_t, uint32_t> VertexIndexes;

	Edge(const uint32_t vertexIndex0, const uint32_t vertexIndex1);

	bool operator==(const Edge& other) const;
	bool operator!=(const Edge& other) const;
};

namespace std
{
	template <>
	struct hash<Edge>
	{
		size_t operator()(const Edge& edge) const
		{
			return hash<uint32_t>()(edge.VertexIndexes.first) ^ (hash<uint32_t>()(edge.VertexIndexes.second) << 1);
		}
	};
}