#pragma once

#include <numbers>

inline constexpr float EPSILON = 1e-5f;
inline constexpr float PI = std::numbers::pi_v<float>;

template <typename T>
bool IsZero(const T value)
{
	return abs(value) < EPSILON;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type
IsOdd(const T value)
{
	return value & 1;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type
IsEven(const T value)
{
	return !IsOdd(value);
}

template <typename T>
T RadToDeg(const T rad)
{
	static constexpr T k = static_cast<T>(180 / PI);
	return k * rad;
}

template <typename T>
T DegToRad(const T deg)
{
	static constexpr T k = static_cast<T>(PI / 180);
	return k * deg;
}