#pragma once

#include "Math/Math.h"

template <typename T>
struct Vector3
{
	T x = 0;
	T y = 0;
	T z = 0;

	T Magnitude() const;
	T MagnitudeSquared() const;
	Vector3 Normalized() const;

	T DotProduct(const Vector3& other) const;
	Vector3 CrossProduct(const Vector3& other) const;

	T AngleBetween(const Vector3& other) const;

	Vector3 ProjectOnto(const Vector3& other) const;

	bool IsZeroVector() const;

	bool IsParallelTo(const Vector3& other) const;
	bool IsOrthogonalTo(const Vector3& other) const;

	bool operator==(const Vector3& other) const;
	bool operator!=(const Vector3& other) const;

	Vector3 operator+() const;
	Vector3 operator-() const;

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;
	Vector3 operator*(const T scalar) const;
	Vector3 operator/(const T scalar) const;

	void operator+=(const Vector3& other);
	void operator-=(const Vector3& other);
	void operator*=(const T scalar);
	void operator/=(const T scalar);
};

template <typename T>
Vector3<T> operator*(const T scalar, const Vector3<T>& vector);

using Vector3i = Vector3<int>;
using Vector3l = Vector3<long long>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

template <typename T>
T Vector3<T>::Magnitude() const
{
	return sqrt(MagnitudeSquared());
}

template <typename T>
T Vector3<T>::MagnitudeSquared() const
{
	return x * x + y * y + z * z;
}

template <typename T>
Vector3<T> Vector3<T>::Normalized() const
{
	const T magnitude = Magnitude();
	ASSERT(magnitude > EPSILON);

	return { x / magnitude, y / magnitude, z / magnitude };
}

template <typename T>
T Vector3<T>::DotProduct(const Vector3& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

template <typename T>
Vector3<T> Vector3<T>::CrossProduct(const Vector3& other) const
{
	return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
}

template <typename T>
T Vector3<T>::AngleBetween(const Vector3& other) const
{
	const T magnitudesMultiplied = Magnitude() * other.Magnitude();
	ASSERT(magnitudesMultiplied > EPSILON);

	return acos(DotProduct(other) / magnitudesMultiplied);
}

template <typename T>
Vector3<T> Vector3<T>::ProjectOnto(const Vector3& other) const
{
	const T otherMagnitudeSquared = other.MagnitudeSquared();
	ASSERT(otherMagnitudeSquared > EPSILON);

	return other * (DotProduct(other) / otherMagnitudeSquared);
}

template <typename T>
bool Vector3<T>::IsZeroVector() const
{
	return IsZero(MagnitudeSquared());
}

template <typename T>
bool Vector3<T>::IsParallelTo(const Vector3& other) const
{
	return CrossProduct(other).IsZeroVector();
}

template <typename T>
bool Vector3<T>::IsOrthogonalTo(const Vector3& other) const
{
	return IsZero(DotProduct(other));
}

template <typename T>
bool Vector3<T>::operator==(const Vector3& other) const
{
	return IsZero(x - other.x)
		&& IsZero(y - other.y)
		&& IsZero(z - other.z);
}

template <typename T>
bool Vector3<T>::operator!=(const Vector3& other) const
{
	return !(*this == other);
}

template <typename T>
Vector3<T> Vector3<T>::operator+() const
{
	return { x, y, z };
}

template <typename T>
Vector3<T> Vector3<T>::operator-() const
{
	return { -x, -y, -z };
}

template <typename T>
Vector3<T> Vector3<T>::operator+(const Vector3& other) const
{
	Vector3 retval(*this);
	retval += other;
	return retval;
}

template <typename T>
Vector3<T> Vector3<T>::operator-(const Vector3& other) const
{
	Vector3 retval(*this);
	retval -= other;
	return retval;
}

template <typename T>
Vector3<T> Vector3<T>::operator*(const T scalar) const
{
	Vector3 retval(*this);
	retval *= scalar;
	return retval;
}

template <typename T>
Vector3<T> operator*(const T scalar, const Vector3<T>& vector)
{
	return vector * scalar;
}

template <typename T>
Vector3<T> Vector3<T>::operator/(const T scalar) const
{
	Vector3 retval(*this);
	retval /= scalar;
	return retval;
}

template <typename T>
void Vector3<T>::operator+=(const Vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

template <typename T>
void Vector3<T>::operator-=(const Vector3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

template <typename T>
void Vector3<T>::operator*=(const T scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

template <typename T>
void Vector3<T>::operator/=(const T scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
}