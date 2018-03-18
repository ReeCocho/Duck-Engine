/**
 * @file vectors.cpp
 * @brief Duck math vector source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vectors.hpp"

namespace dk
{
	Vec_t<2>::Vec_t() : x(0), y(0) {}

	Vec_t<2>::Vec_t(float x_a) : x(x_a), y(x_a) {}

	Vec_t<2>::Vec_t(float x_a, float y_a) : x(x_a), y(y_a) {}

	Vec_t<2>::Vec_t(const Vec_t<2>& other) : x(other.x), y(other.y) {}

	Vec_t<2>& Vec_t<2>::operator=(const Vec_t<2>& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	std::string Vec_t<2>::to_string() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}

	constexpr size_t Vec_t<2>::size() const
	{
		return 2;
	}

	bool Vec_t<2>::operator==(const Vec_t<2>& other) const
	{
		return x == other.x && y == other.y;
	}

	bool Vec_t<2>::operator!=(const Vec_t<2>& other) const
	{
		return x != other.x || y != other.y;
	}

	float Vec_t<2>::magnitude() const
	{
		return static_cast<float>(std::sqrt(square_magnitude()));
	}



	Vec_t<3>::Vec_t() : x(0), y(0), z(0) {}

	Vec_t<3>::Vec_t(float val) : x(val), y(val), z(val) {}

	Vec_t<3>::Vec_t(float x_a, float y_a, float z_a) : x(x_a), y(y_a), z(z_a) {}

	Vec_t<3>::Vec_t(const Vec_t<2>& other) : x(other.x), y(other.y), z(0) {}

	Vec_t<3>::Vec_t(const Vec_t<3>& other) : x(other.x), y(other.y), z(other.z) {}

	Vec_t<3>& Vec_t<3>::operator=(const Vec_t<3>& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator=(const Vec_t<2>& other)
	{
		x = other.x;
		y = other.y;
		z = 0.0f;
		return *this;
	}

	std::string Vec_t<3>::to_string() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}

	constexpr size_t Vec_t<3>::size() const
	{
		return 3;
	}

	bool Vec_t<3>::operator==(const Vec_t<3>& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool Vec_t<3>::operator==(const Vec_t<2>& other) const
	{
		return x == other.x && y == other.y && z == 0.0f;
	}

	bool Vec_t<3>::operator!=(const Vec_t<3>& other) const
	{
		return x != other.x || y != other.y || z != other.z;
	}

	bool Vec_t<3>::operator!=(const Vec_t<2>& other) const
	{
		return x != other.x || y != other.y || z != 0.0f;
	}

	float Vec_t<3>::magnitude() const
	{
		return static_cast<float>(std::sqrt(square_magnitude()));
	}



	Vec_t<4>::Vec_t() : x(0), y(0), z(0), w(0) {}

	Vec_t<4>::Vec_t(float val) : x(val), y(val), z(val), w(val) {}

	Vec_t<4>::Vec_t(float x_a, float y_a, float z_a, float w_a) : x(x_a), y(y_a), z(z_a), w(w_a) {}

	Vec_t<4>::Vec_t(const Vec_t<2>& other) : x(other.x), y(other.y), z(0), w(0) {}

	Vec_t<4>::Vec_t(const Vec_t<3>& other) : x(other.x), y(other.y), z(other.z), w(0) {}

	Vec_t<4>::Vec_t(const Vec_t<4>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	Vec_t<4>& Vec_t<4>::operator=(const Vec_t<4>& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator=(const Vec_t<3>& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = 0.0f;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator=(const Vec_t<2>& other)
	{
		x = other.x;
		y = other.y;
		z = 0.0f;
		w = 0.0f;
		return *this;
	}

	std::string Vec_t<4>::to_string() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
	}

	constexpr size_t Vec_t<4>::size() const
	{
		return 4;
	}

	bool Vec_t<4>::operator==(const Vec_t<4>& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Vec_t<4>::operator==(const Vec_t<3>& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == 0.0f;
	}

	bool Vec_t<4>::operator==(const Vec_t<2>& other) const
	{
		return x == other.x && y == other.y && z == 0.0f && w == 0.0f;
	}

	bool Vec_t<4>::operator!=(const Vec_t<4>& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	bool Vec_t<4>::operator!=(const Vec_t<3>& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != 0.0f;
	}

	bool Vec_t<4>::operator!=(const Vec_t<2>& other) const
	{
		return x != other.x || y != other.y || z != 0.0f || w != 0.0f;
	}

	float Vec_t<4>::magnitude() const
	{
		return static_cast<float>(std::sqrt(square_magnitude()));
	}
}

#if DUCK_USE_SIMD
namespace dk
{
	Vec_t<2> Vec_t<2>::operator+(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	Vec_t<2> Vec_t<2>::operator-(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	Vec_t<2> Vec_t<2>::operator*(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	Vec_t<2> Vec_t<2>::operator/(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	Vec_t<2>& Vec_t<2>::operator+=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<2>& Vec_t<2>::operator-=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<2>& Vec_t<2>::operator*=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<2>& Vec_t<2>::operator/=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	float Vec_t<2>::square_magnitude() const
	{
		__m128 v1 = { x, y, 0.0f, 0.0f };
		v1 = _mm_mul_ps(v1, v1);
		return v1.m128_f32[0] + v1.m128_f32[1];
	}

	float Vec_t<2>::dot(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return v1.m128_f32[0] + v1.m128_f32[1];
	}

	Vec_t<2>& Vec_t<2>::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}

	Vec_t<2> Vec_t<2>::normalized() const
	{
		float mag = magnitude();
		if (mag == 0.0f) return Vec_t<2>(0);
		__m128 v1 = _mm_div_ps({ x, y, 0, 0 }, { mag, mag, mag, mag });
		return Vec_t<2>(v1.m128_f32[0], v1.m128_f32[1]);
	}



	Vec_t<3> Vec_t<3>::operator+(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator+(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator-(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator-(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator*(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator*(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, 0 }, { other.x, other.y, 1.0f, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator/(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3> Vec_t<3>::operator/(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, 0 }, { other.x, other.y, 1.0f, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	Vec_t<3>& Vec_t<3>::operator+=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator+=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator-=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator-=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator*=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator*=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, 1, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator/=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator/=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, 1, 0 });
		return *this;
	}

	float Vec_t<3>::square_magnitude() const
	{
		__m128 v1 = { x, y, z, 0.0f };
		v1 = _mm_mul_ps(v1, v1);
		return v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2];
	}

	float Vec_t<3>::dot(const Vec_t<3>& other) const
	{
		__m128 v1 = { x, y, z, 0.0f };
		__m128 v2 = { other.x, other.y, other.z, 0.0f };
		__m128 v3 = _mm_mul_ps(v1, v2);
		return v3.m128_f32[0] + v3.m128_f32[1] + v3.m128_f32[2];
	}

	Vec_t<3>& Vec_t<3>::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { mag, mag, mag, mag });
		return *this;
	}

	Vec_t<3> Vec_t<3>::normalized() const
	{
		float mag = magnitude();
		if (mag == 0.0f) return Vec_t<3>(0);
		__m128 v1 = { x, y, z, 0 };
		v1 = _mm_div_ps(v1, { mag, mag, mag, mag });
		return Vec_t<3>(v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2]);
	}



	Vec_t<4> Vec_t<4>::operator+(const Vec_t<4>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator+(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator+(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator-(const Vec_t<4>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator-(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator-(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator*(const Vec_t<4>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator*(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { other.x, other.y, other.z, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator*(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, 1, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator/(const Vec_t<4>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator/(const Vec_t<3>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { other.x, other.y, other.z, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4> Vec_t<4>::operator/(const Vec_t<2>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { other.x, other.y, 1, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Vec_t<4>& Vec_t<4>::operator+=(const Vec_t<4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator+=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator+=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator-=(const Vec_t<4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator-=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator-=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator*=(const Vec_t<4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator*=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, other.z, 1 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator*=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, 1, 1 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator/=(const Vec_t<4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator/=(const Vec_t<3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, other.z, 1 });
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator/=(const Vec_t<2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, 1, 1 });
		return *this;
	}

	float Vec_t<4>::square_magnitude() const
	{
		__m128 v1 = { x, y, z, w };
		v1 = _mm_mul_ps(v1, v1);
		return v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3];
	}

	float Vec_t<4>::dot(const Vec_t<4>& other) const
	{
		__m128 v1 = { x, y, z, w };
		__m128 v2 = { other.x, other.y, other.z, other.w };
		v1 = _mm_mul_ps(v1, v2);
		return v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3];
	}

	Vec_t<4>& Vec_t<4>::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}

	Vec_t<4> Vec_t<4>::normalized() const
	{
		float mag = magnitude();
		if (mag == 0.0f) return Vec_t<4>(0);
		__m128 v1 = { x, y, z, w };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		return Vec_t<4>(v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3]);
	}
}
#else
namespace dk
{
	Vec_t<2> Vec_t<2>::operator+(const Vec_t<2>& other) const
	{
		return { x + other.x, y + other.y };
	}

	Vec_t<2> Vec_t<2>::operator-(const Vec_t<2>& other) const
	{
		return { x - other.x, y - other.y };
	}

	Vec_t<2> Vec_t<2>::operator*(const Vec_t<2>& other) const
	{
		return { x * other.x, y * other.y };
	}

	Vec_t<2> Vec_t<2>::operator/(const Vec_t<2>& other) const
	{
		return { x / other.x, y / other.y };
	}

	Vec_t<2>& Vec_t<2>::operator+=(const Vec_t<2>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec_t<2>& Vec_t<2>::operator-=(const Vec_t<2>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec_t<2>& Vec_t<2>::operator*=(const Vec_t<2>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vec_t<2>& Vec_t<2>::operator/=(const Vec_t<2>& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	float Vec_t<2>::square_magnitude() const
	{
		return (x*x) + (y*y);
	}

	float Vec_t<2>::dot(const Vec_t<2>& other) const
	{
		return (x * other.x) + (y * other.y);
	}

	Vec_t<2>& Vec_t<2>::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		x /= mag;
		y /= mag;
		return *this;
	}

	Vec_t<2> Vec_t<2>::normalized() const
	{
		float mag = magnitude();
		if (mag == 0) return Vec_t<2>(0);
		Vec_t<2> new_vec = *this;
		new_vec.x /= mag;
		new_vec.y /= mag;
		return new_vec;
	}



	Vec_t<3> Vec_t<3>::operator+(const Vec_t<3>& other) const
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	Vec_t<3> Vec_t<3>::operator+(const Vec_t<2>& other) const
	{
		return { x + other.x, y + other.y, z };
	}

	Vec_t<3> Vec_t<3>::operator-(const Vec_t<3>& other) const
	{
		return { x - other.x, y - other.y, z - other.z };
	}

	Vec_t<3> Vec_t<3>::operator-(const Vec_t<2>& other) const
	{
		return { x - other.x, y - other.y, z };
	}

	Vec_t<3> Vec_t<3>::operator*(const Vec_t<3>& other) const
	{
		return { x * other.x, y * other.y, z * other.z };
	}

	Vec_t<3> Vec_t<3>::operator*(const Vec_t<2>& other) const
	{
		return { x * other.x, y * other.y, z };
	}

	Vec_t<3> Vec_t<3>::operator/(const Vec_t<3>& other) const
	{
		return { x / other.x, y / other.y, z / other.z };
	}

	Vec_t<3> Vec_t<3>::operator/(const Vec_t<2>& other) const
	{
		return { x / other.x, y / other.y, z };
	}

	Vec_t<3>& Vec_t<3>::operator+=(const Vec_t<3>& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator+=(const Vec_t<2>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator-=(const Vec_t<3>& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator-=(const Vec_t<2>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator*=(const Vec_t<3>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator*=(const Vec_t<2>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator/=(const Vec_t<3>& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vec_t<3>& Vec_t<3>::operator/=(const Vec_t<2>& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	float Vec_t<3>::square_magnitude() const
	{
		return (x*x) + (y*y) + (z*z);
	}

	float Vec_t<3>::dot(const Vec_t<3>& other) const
	{
		return (x*other.x) + (y*other.y) + (z*other.z);
	}

	Vec_t<3>& Vec_t<3>::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		x /= mag;
		y /= mag;
		z /= mag;
		return *this;
	}

	Vec_t<3> Vec_t<3>::normalized() const
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		Vec_t<3> new_vec = *this;
		new_vec.x /= mag;
		new_vec.y /= mag;
		new_vec.z /= mag;
		return new_vec;
	}



	Vec_t<4> Vec_t<4>::operator+(const Vec_t<4>& other) const
	{
		return { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	Vec_t<4> Vec_t<4>::operator+(const Vec_t<3>& other) const
	{
		return { x + other.x, y + other.y, z + other.z, w };
	}

	Vec_t<4> Vec_t<4>::operator+(const Vec_t<2>& other) const
	{
		return { x + other.x, y + other.y, z, w };
	}

	Vec_t<4> Vec_t<4>::operator-(const Vec_t<4>& other) const
	{
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	Vec_t<4> Vec_t<4>::operator-(const Vec_t<3>& other) const
	{
		return { x - other.x, y - other.y, z - other.z, w };
	}

	Vec_t<4> Vec_t<4>::operator-(const Vec_t<2>& other) const
	{
		return { x - other.x, y - other.y, z, w };
	}

	Vec_t<4> Vec_t<4>::operator*(const Vec_t<4>& other) const
	{
		return { x * other.x, y * other.y, z * other.z, w * other.w };
	}

	Vec_t<4> Vec_t<4>::operator*(const Vec_t<3>& other) const
	{
		return { x * other.x, y * other.y, z * other.z, w };
	}

	Vec_t<4> Vec_t<4>::operator*(const Vec_t<2>& other) const
	{
		return { x * other.x, y * other.y, z, w };
	}

	Vec_t<4> Vec_t<4>::operator/(const Vec_t<4>& other) const
	{
		return { x / other.x, y / other.y, z / other.z, w / other.w };
	}

	Vec_t<4> Vec_t<4>::operator/(const Vec_t<3>& other) const
	{
		return { x / other.x, y / other.y, z / other.z, w };
	}

	Vec_t<4> Vec_t<4>::operator/(const Vec_t<2>& other) const
	{
		return { x / other.x, y / other.y, z, w };
	}

	Vec_t<4>& Vec_t<4>::operator+=(const Vec_t<4>& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator+=(const Vec_t<3>& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator+=(const Vec_t<2>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator-=(const Vec_t<4>& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator-=(const Vec_t<3>& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator-=(const Vec_t<2>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator*=(const Vec_t<4>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator*=(const Vec_t<3>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator*=(const Vec_t<2>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator/=(const Vec_t<4>& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator/=(const Vec_t<3>& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vec_t<4>& Vec_t<4>::operator/=(const Vec_t<2>& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	float Vec_t<4>::square_magnitude() const
	{
		return (x*x) + (y*y) + (z*z) + (w*w);
	}

	float Vec_t<4>::dot(const Vec_t<4>& other) const
	{
		return (x*other.x) + (y*other.y) + (z*other.z) + (w*other.w);
	}

	Vec_t<4>& Vec_t<4>::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		x /= mag;
		y /= mag;
		z /= mag;
		w /= mag;
		return *this;
	}

	Vec_t<4> Vec_t<4>::normalized() const
	{
		float mag = magnitude();
		if (mag == 0.0f) return Vec_t<4>(0);
		Vec_t<4> new_vec = *this;
		new_vec.x /= mag;
		new_vec.y /= mag;
		new_vec.z /= mag;
		new_vec.w /= mag;
		return new_vec;
	}
}
#endif