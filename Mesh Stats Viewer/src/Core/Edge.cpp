#include "pch.h"
#include "Core/Edge.h"

Edge::Edge(const uint32_t vertexIndex0, const uint32_t vertexIndex1)
{
	if (vertexIndex0 <= vertexIndex1)
	{
		VertexIndexes.first = vertexIndex0;
		VertexIndexes.second = vertexIndex1;
	}
	else
	{
		VertexIndexes.first = vertexIndex1;
		VertexIndexes.second = vertexIndex0;
	}
}

bool Edge::operator==(const Edge& other) const
{
	return VertexIndexes.first == other.VertexIndexes.first
		&& VertexIndexes.second == other.VertexIndexes.second;
}

bool Edge::operator!=(const Edge& other) const
{
	return !(*this == other);
}