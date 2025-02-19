#pragma once

#include "Math/Math.h"
#include "Math/Vector3.h"

template <typename T>
struct Ray3
{
	Vector3<T> Origin;
	Vector3<T> Direction;

	Ray3(const Vector3<T>& origin, const Vector3<T>& direction);

	bool IntersectsRay(const Ray3& other) const;
	std::optional<Vector3<T>> PointOfIntersectionWithRay(const Ray3& other) const;

	bool IntersectsTriangle(const Vector3<T>& vertex0, const Vector3<T>& vertex1, const Vector3<T>& vertex2) const;
	std::optional<Vector3<T>> PointOfIntersectionWithTriangle(const Vector3<T>& vertex0, const Vector3<T>& vertex1, const Vector3<T>& vertex2) const;

	bool IsPointOnRay(const Vector3<T>& point) const;
	Vector3<T> GetPointOnRay(const float t) const;

	T DistanceTo(const Vector3<T>& point) const;
	T DistanceTo(const Ray3& other) const;

	T AngleBetween(const Ray3& other) const;

	bool IsParallelTo(const Ray3& other) const;
	bool IsOrthogonalTo(const Ray3& other) const;

	bool operator==(const Ray3& other) const;
	bool operator!=(const Ray3& other) const;

private:
	std::pair<Vector3<T>, Vector3<T>> GetClosestPointsWith(const Ray3& other) const;
};

using Ray3f = Ray3<float>;
using Ray3d = Ray3<double>;

template <typename T>
Ray3<T>::Ray3(const Vector3<T>& origin, const Vector3<T>& direction)
	: Origin(origin)
	, Direction(direction)
{
	ASSERT(!Direction.IsZeroVector());
}

template <typename T>
bool Ray3<T>::IntersectsRay(const Ray3& other) const
{
	return PointOfIntersectionWithRay(other).has_value();
}

template <typename T>
std::optional<Vector3<T>> Ray3<T>::PointOfIntersectionWithRay(const Ray3& other) const
{
	if (IsParallelTo(other)) return {};

	const auto closestPoints = GetClosestPointsWith(other);
	return closestPoints.first == closestPoints.second ? closestPoints.first : std::optional<Vector3<T>>();
}

template <typename T>
bool Ray3<T>::IntersectsTriangle(const Vector3<T>& vertex0, const Vector3<T>& vertex1, const Vector3<T>& vertex2) const
{
	return PointOfIntersectionWithTriangle(vertex0, vertex1, vertex2).has_value();
}

// Moller–Trumbore ray-triangle intersection algorithm (two-sided triangles version)
// For more information see: doc\algorithms\fast_minimum_storage_ray_triangle_intersection.pdf
template <typename T>
std::optional<Vector3<T>> Ray3<T>::PointOfIntersectionWithTriangle(const Vector3<T>& vertex0, const Vector3<T>& vertex1, const Vector3<T>& vertex2) const
{
	const auto edge1 = vertex1 - vertex0;
	const auto edge2 = vertex2 - vertex0;
	const auto pVector = Direction.CrossProduct(edge2);

	const float determinant = pVector.DotProduct(edge1);
	if (IsZero(determinant)) // Ray is parallel to the triangle
		return {};

	const float inverseDeterminant = 1.f / determinant;

	const auto tVector = Origin - vertex0;
	const float u = pVector.DotProduct(tVector) * inverseDeterminant;
	if (u < 0.f || 1.f < u) // Point is outside of the triangle
		return {};

	const auto qVector = tVector.CrossProduct(edge1);
	const float v = qVector.DotProduct(Direction) * inverseDeterminant;
	if (v < 0.f || 1.f < u + v) // Point is outside of the triangle
		return {};

	const float t = qVector.DotProduct(edge2) * inverseDeterminant;
	if (t < 0.f) // Point is in the opposite direction of the ray
		return {};

	return GetPointOnRay(t);
}

template <typename T>
bool Ray3<T>::IsPointOnRay(const Vector3<T>& point) const
{
	return Direction.IsParallelTo(point - Origin);
}

template <typename T>
Vector3<T> Ray3<T>::GetPointOnRay(const float t) const
{
	return Origin + Direction * t;
}

template <typename T>
T Ray3<T>::DistanceTo(const Vector3<T>& point) const
{
	const T directionMagnitude = Direction.Magnitude();
	ASSERT(directionMagnitude > EPSILON);

	return Direction.CrossProduct(point - Origin).Magnitude() / directionMagnitude;
}

template <typename T>
T Ray3<T>::DistanceTo(const Ray3& other) const
{
	const auto closestPoints = GetClosestPointsWith(other);
	return (closestPoints.second - closestPoints.first).Magnitude();
}

template <typename T>
T Ray3<T>::AngleBetween(const Ray3& other) const
{
	return Direction.AngleBetween(other.Direction);
}

template <typename T>
bool Ray3<T>::IsParallelTo(const Ray3& other) const
{
	return Direction.IsParallelTo(other.Direction);
}

template <typename T>
bool Ray3<T>::IsOrthogonalTo(const Ray3& other) const
{
	return Direction.IsOrthogonalTo(other.Direction);
}

template <typename T>
bool Ray3<T>::operator==(const Ray3& other) const
{
	return IsPointOnRay(other.Origin)
		&& Direction.IsParallelTo(other.Direction);
}

template <typename T>
bool Ray3<T>::operator!=(const Ray3& other) const
{
	return !(*this == other);
}

template <typename T>
std::pair<Vector3<T>, Vector3<T>> Ray3<T>::GetClosestPointsWith(const Ray3& other) const
{
	const Vector3<T> crossProduct = Direction.CrossProduct(other.Direction);
	const Vector3<T> originVector = other.Origin - Origin;
	T t1, t2;

	if (!IsZero(crossProduct.x))
	{
		t1 = (other.Direction.z * originVector.y - other.Direction.y * originVector.z) / crossProduct.x;
		t2 = (this->Direction.z * originVector.y - this->Direction.y * originVector.z) / crossProduct.x;
	}
	else if (!IsZero(crossProduct.y))
	{
		t1 = (other.Direction.z * originVector.x - other.Direction.x * originVector.z) / -crossProduct.y;
		t2 = (this->Direction.z * originVector.x - this->Direction.x * originVector.z) / -crossProduct.y;
	}
	else if (!IsZero(crossProduct.z))
	{
		t1 = (other.Direction.y * originVector.x - other.Direction.x * originVector.y) / crossProduct.z;
		t2 = (this->Direction.y * originVector.x - this->Direction.x * originVector.y) / crossProduct.z;
	}
	else
	{
		const T otherDirectionMagnitudeSquared = other.Direction.MagnitudeSquared();
		ASSERT(otherDirectionMagnitudeSquared > EPSILON);

		t1 = 0;
		t2 = -originVector.DotProduct(other.Direction) / otherDirectionMagnitudeSquared;
	}

	return { GetPointOnRay(t1), other.GetPointOnRay(t2) };
}