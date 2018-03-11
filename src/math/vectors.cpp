/**
 * @file vectors.cpp
 * @brief Duck math vector source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vectors.hpp"

namespace dk
{
#if DUCK_USE_SIMD

	// vec2f
	template<>
	template<>
	vec_t<float, 2> vec_t<float, 2>::operator+(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	template<>
	template<>
	vec_t<float, 2> vec_t<float, 2>::operator-(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	template<>
	template<>
	vec_t<float, 2> vec_t<float, 2>::operator*(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	template<>
	template<>
	vec_t<float, 2> vec_t<float, 2>::operator/(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1] };
	}

	template<>
	template<>
	vec_t<float, 2>& vec_t<float, 2>::operator+=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 2>& vec_t<float, 2>::operator-=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 2>& vec_t<float, 2>::operator*=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 2>& vec_t<float, 2>::operator/=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	RealNumber vec_t<float, 2>::square_magnitude() const
	{
		__m128 v1 = { x, y, 0.0f, 0.0f };
		v1 = _mm_mul_ps(v1, v1);
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1]);
	}

	template<>
	template<>
	RealNumber vec_t<float, 2>::dot(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, 0, 0 }, { other.x, other.y, 0, 0 });
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1]);
	}

	template<>
	vec_t<float, 2>& vec_t<float, 2>::normalize()
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}

	template<>
	vec_t<float, 2> vec_t<float, 2>::normalized() const
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return vec_t<float, 2>(0);
		__m128 v1 = _mm_div_ps({ x, y, 0, 0 }, { mag, mag, mag, mag });
		return vec_t<float, 2>(v1.m128_f32[0], v1.m128_f32[1]);
	}



	// vec2i
	template<>
	template<>
	vec_t<int32_t, 2> vec_t<int32_t, 2>::operator+(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		v1 = _mm_add_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1] };
	}

	template<>
	template<>
	vec_t<int32_t, 2> vec_t<int32_t, 2>::operator-(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		v1 = _mm_sub_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1] };
	}

	template<>
	template<>
	vec_t<int32_t, 2> vec_t<int32_t, 2>::operator*(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		v1 = _mm_mul_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1] };
	}

	template<>
	template<>
	vec_t<int32_t, 2> vec_t<int32_t, 2>::operator/(const vec_t<int32_t, 2>& other) const
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 0.0f, 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);

		return 
		{ 
			static_cast<int32_t>(v3.m128_f32[0]), 
			static_cast<int32_t>(v3.m128_f32[1])
		};
	}

	template<>
	template<>
	vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator+=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		*v1 = _mm_add_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator-=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		*v1 = _mm_sub_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator*=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		*v1 = _mm_mul_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator/=(const vec_t<int32_t, 2>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 0.0f, 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		return *this;
	}

	template<>
	RealNumber vec_t<int32_t, 2>::square_magnitude() const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1 = _mm_mul_epi32(v1, v1);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1]);
	}

	template<>
	template<>
	RealNumber vec_t<int32_t, 2>::dot(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		v1 = _mm_mul_epi32(v1, v2);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1]);
	}

	template<>
	vec_t<int32_t, 2>& vec_t<int32_t, 2>::normalize()
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return *this;
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f };
		__m128 v2 = { mag, mag, mag, mag };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		return *this;
	}

	template<>
	vec_t<int32_t, 2> vec_t<int32_t, 2>::normalized() const
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return vec_t<int32_t, 2>(0);
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), 0, 0 };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		return vec_t<int32_t, 2>(static_cast<int32_t>(v1.m128_f32[0]), static_cast<int32_t>(v1.m128_f32[1]));
	}



	// vec3f
	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator+(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator+(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator-(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator-(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, 0 }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator*(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator*(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, 0 }, { other.x, other.y, 1, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator/(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, 0 }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3> vec_t<float, 3>::operator/(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, 0 }, { other.x, other.y, 1, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2] };
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator+=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator+=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator-=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator-=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator*=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator*=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, 1, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator/=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 3>& vec_t<float, 3>::operator/=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, 1, 0 });
		return *this;
	}

	template<>
	RealNumber vec_t<float, 3>::square_magnitude() const
	{
		__m128 v1 = { x, y, z, 0.0f };
		v1 = _mm_mul_ps(v1, v1);
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2]);
	}

	template<>
	template<>
	RealNumber vec_t<float, 3>::dot(const vec_t<float, 3>& other) const
	{
		__m128 v1 = { x, y, z, 0.0f };
		__m128 v2 = { other.x, other.y, other.z, 0.0f };
		__m128 v3 = _mm_mul_ps(v1, v2);
		return static_cast<RealNumber>(v3.m128_f32[0] + v3.m128_f32[1] + v3.m128_f32[2]);
	}

	template<>
	vec_t<float, 3>& vec_t<float, 3>::normalize()
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { mag, mag, mag, mag });
		return *this;
	}

	template<>
	vec_t<float, 3> vec_t<float, 3>::normalized() const
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return vec_t<float, 3>(0);
		__m128 v1 = { x, y, z, 0 };
		v1 = _mm_div_ps(v1, { mag, mag, mag, mag });
		return vec_t<float, 3>(v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2]);
	}



	// vec3i
	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator+(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;

		v1 = _mm_add_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2] };
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator+(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = 0;

		v1 = _mm_add_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2] };
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator-(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;

		v1 = _mm_sub_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2] };
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator-(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = 0;

		v1 = _mm_sub_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2] };
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator*(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;

		v1 = _mm_mul_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2] };
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator*(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = 1;

		v1 = _mm_mul_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2] };
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator/(const vec_t<int32_t, 3>& other) const
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), static_cast<float>(other.z), 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);

		return 
		{ 
			static_cast<int32_t>(v3.m128_f32[0]), 
			static_cast<int32_t>(v3.m128_f32[1]),
			static_cast<int32_t>(v3.m128_f32[2])
		};
	}

	template<>
	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::operator/(const vec_t<int32_t, 2>& other) const
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 0.0f, 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);

		return
		{
			static_cast<int32_t>(v3.m128_f32[0]),
			static_cast<int32_t>(v3.m128_f32[1]),
			z
		};
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator+=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		*v1 = _mm_add_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator+=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = 0;
		*v1 = _mm_add_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator-=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		*v1 = _mm_sub_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator-=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = 0;
		*v1 = _mm_sub_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator*=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		*v1 = _mm_mul_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator*=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = 1;
		*v1 = _mm_mul_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator/=(const vec_t<int32_t, 3>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), static_cast<float>(other.z), 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		z = static_cast<int32_t>(v3.m128_f32[2]);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator/=(const vec_t<int32_t, 2>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 1.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 1.0f, 1.0f };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		return *this;
	}

	template<>
	RealNumber vec_t<int32_t, 3>::square_magnitude() const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1 = _mm_mul_epi32(v1, v1);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1] + v1.m128i_i32[2]);
	}

	template<>
	template<>
	RealNumber vec_t<int32_t, 3>::dot(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;

		v1 = _mm_mul_epi32(v1, v2);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1] + v1.m128i_i32[2]);
	}

	template<>
	vec_t<int32_t, 3>& vec_t<int32_t, 3>::normalize()
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return *this;
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0.0f };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v1.m128_f32[0]);
		y = static_cast<int32_t>(v1.m128_f32[1]);
		z = static_cast<int32_t>(v1.m128_f32[2]);
		return *this;
	}

	template<>
	vec_t<int32_t, 3> vec_t<int32_t, 3>::normalized() const
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return vec_t<int32_t, 3>(0);
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0 };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		return vec_t<int32_t, 3>(
			static_cast<int32_t>(v1.m128_f32[0]), 
			static_cast<int32_t>(v1.m128_f32[1]), 
			static_cast<int32_t>(v1.m128_f32[2]));
	}



	// vec4f
	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator+(const vec_t<float, 4>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator+(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator+(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator-(const vec_t<float, 4>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator-(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, other.z, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator-(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, 0, 0 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator*(const vec_t<float, 4>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator*(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { other.x, other.y, other.z, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator*(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, 1, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator/(const vec_t<float, 4>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator/(const vec_t<float, 3>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { other.x, other.y, other.z, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4> vec_t<float, 4>::operator/(const vec_t<float, 2>& other) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { other.x, other.y, 1, 1 });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator+=(const vec_t<float, 4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator+=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator+=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator-=(const vec_t<float, 4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator-=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator-=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, 0, 0 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator*=(const vec_t<float, 4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator*=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, other.z, 1 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator*=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { other.x, other.y, 1, 1 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator/=(const vec_t<float, 4>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator/=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, other.z, 1 });
		return *this;
	}

	template<>
	template<>
	vec_t<float, 4>& vec_t<float, 4>::operator/=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { other.x, other.y, 1, 1 });
		return *this;
	}

	template<>
	RealNumber vec_t<float, 4>::square_magnitude() const
	{
		__m128 v1 = { x, y, z, w };
		v1 = _mm_mul_ps(v1, v1);
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3]);
	}

	template<>
	template<>
	RealNumber vec_t<float, 4>::dot(const vec_t<float, 4>& other) const
	{
		__m128 v1 = { x, y, z, w };
		__m128 v2 = { other.x, other.y, other.z, other.w };
		v1 = _mm_mul_ps(v1, v2);
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3]);
	}

	template<>
	vec_t<float, 4>& vec_t<float, 4>::normalize()
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}

	template<>
	vec_t<float, 4> vec_t<float, 4>::normalized() const
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return vec_t<float, 4>(0);
		__m128 v1 = { x, y, z, w };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		return vec_t<float, 4>(v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3]);
	}



	// vec4i
	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator+(const vec_t<int32_t, 4>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v1.m128i_i32[3] = other.w;

		v1 = _mm_add_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator+(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v1.m128i_i32[3] = 0;

		v1 = _mm_add_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator+(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = 0;
		v1.m128i_i32[3] = 0;

		v1 = _mm_add_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator-(const vec_t<int32_t, 4>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v1.m128i_i32[3] = other.w;

		v1 = _mm_sub_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator-(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = 0;

		v1 = _mm_sub_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator-(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = 0;
		v1.m128i_i32[3] = 0;

		v1 = _mm_sub_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator*(const vec_t<int32_t, 4>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v1.m128i_i32[3] = other.w;

		v1 = _mm_mul_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator*(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v1.m128i_i32[3] = 1;

		v1 = _mm_mul_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator*(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = 1;
		v1.m128i_i32[3] = 1;

		v1 = _mm_mul_epi32(v1, v2);
		return { v1.m128i_i32[0], v1.m128i_i32[1], v1.m128i_i32[2], v1.m128i_i32[3] };
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator/(const vec_t<int32_t, 4>& other) const
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), static_cast<float>(other.z), static_cast<float>(other.w) };
		v1 = _mm_div_ps(v1, v2);

		return
		{
			static_cast<int32_t>(v1.m128_f32[0]),
			static_cast<int32_t>(v1.m128_f32[1]),
			static_cast<int32_t>(v1.m128_f32[2]),
			static_cast<int32_t>(v1.m128_f32[3])
		};
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator/(const vec_t<int32_t, 3>& other) const
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), static_cast<float>(other.z), 1 };
		v1 = _mm_div_ps(v1, v2);

		return
		{
			static_cast<int32_t>(v1.m128_f32[0]),
			static_cast<int32_t>(v1.m128_f32[1]),
			static_cast<int32_t>(v1.m128_f32[2]),
			static_cast<int32_t>(v1.m128_f32[3])
		};
	}

	template<>
	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::operator/(const vec_t<int32_t, 2>& other) const
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 1, 1 };
		v1 = _mm_div_ps(v1, v2);

		return
		{
			static_cast<int32_t>(v1.m128_f32[0]),
			static_cast<int32_t>(v1.m128_f32[1]),
			static_cast<int32_t>(v1.m128_f32[2]),
			static_cast<int32_t>(v1.m128_f32[3])
		};
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator+=(const vec_t<int32_t, 4>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = other.w;
		*v1 = _mm_add_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator+=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = 0;
		*v1 = _mm_add_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator+=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = 0;
		v2.m128i_i32[3] = 0;
		*v1 = _mm_add_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator-=(const vec_t<int32_t, 4>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = other.w;
		*v1 = _mm_sub_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator-=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = 0;
		*v1 = _mm_sub_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator-=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = 0;
		v2.m128i_i32[3] = 0;
		*v1 = _mm_sub_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator*=(const vec_t<int32_t, 4>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = other.w;
		*v1 = _mm_mul_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator*=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;
		v2.m128i_i32[3] = 1;
		*v1 = _mm_mul_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator*=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = 1;
		v2.m128i_i32[3] = 1;
		*v1 = _mm_mul_epi32(*v1, v2);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator/=(const vec_t<int32_t, 4>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), static_cast<float>(other.z), static_cast<float>(other.w) };
		v1 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v1.m128_f32[0]);
		y = static_cast<int32_t>(v1.m128_f32[1]);
		z = static_cast<int32_t>(v1.m128_f32[2]);
		w = static_cast<int32_t>(v1.m128_f32[3]);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator/=(const vec_t<int32_t, 3>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), static_cast<float>(other.z), 0 };
		v1 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v1.m128_f32[0]);
		y = static_cast<int32_t>(v1.m128_f32[1]);
		z = static_cast<int32_t>(v1.m128_f32[2]);
		return *this;
	}

	template<>
	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::operator/=(const vec_t<int32_t, 2>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 0, 0};
		v1 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v1.m128_f32[0]);
		y = static_cast<int32_t>(v1.m128_f32[1]);
		return *this;
	}

	template<>
	RealNumber vec_t<int32_t, 4>::square_magnitude() const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;
		v1 = _mm_mul_epi32(v1, v1);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1] + v1.m128i_i32[2] + v1.m128i_i32[3]);
	}

	template<>
	template<>
	RealNumber vec_t<int32_t, 4>::dot(const vec_t<int32_t, 4>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		v1.m128i_i32[3] = w;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;
		v1.m128i_i32[2] = other.z;
		v1.m128i_i32[3] = other.w;

		v1 = _mm_mul_epi32(v1, v2);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1] + v1.m128i_i32[2] + v1.m128i_i32[3]);
	}

	template<>
	vec_t<int32_t, 4>& vec_t<int32_t, 4>::normalize()
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return *this;
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v1.m128_f32[0]);
		y = static_cast<int32_t>(v1.m128_f32[1]);
		z = static_cast<int32_t>(v1.m128_f32[2]);
		w = static_cast<int32_t>(v1.m128_f32[3]);
		return *this;
	}

	template<>
	vec_t<int32_t, 4> vec_t<int32_t, 4>::normalized() const
	{
		float mag = static_cast<float>(magnitude());
		if (mag == 0) return vec_t<int32_t, 4>(0);
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w) };
		__m128 v2 = { mag, mag, mag, mag };
		v1 = _mm_div_ps(v1, v2);
		return vec_t<int32_t, 4>(
			static_cast<int32_t>(v1.m128_f32[0]), 
			static_cast<int32_t>(v1.m128_f32[1]), 
			static_cast<int32_t>(v1.m128_f32[2]),
			static_cast<int32_t>(v1.m128_f32[3]));
	}

#endif
}