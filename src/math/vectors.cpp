#include "vectors.hpp"

namespace dk
{
#if DUCK_USE_SIMD

	// vec2f
	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2> vec_t<float, 2>::operator+(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_add_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2> vec_t<float, 2>::operator-(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_sub_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2> vec_t<float, 2>::operator*(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_mul_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2> vec_t<float, 2>::operator/(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_div_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2>& vec_t<float, 2>::operator+=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_add_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2>& vec_t<float, 2>::operator-=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_sub_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2>& vec_t<float, 2>::operator*=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_mul_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 2>& vec_t<float, 2>::operator/=(const vec_t<float, 2>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_div_ps(*v1, *v2);
		return *this;
	}

	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<float, 2>::square_magnitude() const
	{
		__m128 v1 = { x, y, 0.0f, 0.0f };
		v1 = _mm_mul_ps(v1, v1);
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1]);
	}

	template<>
	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<float, 2>::dot(const vec_t<float, 2>& other) const
	{
		__m128 v1 = { x, y, 0.0f, 0.0f };
		__m128 v2 = { other.x, other.y, 0.0f, 0.0f };
		__m128 v3 = _mm_mul_ps(v1, v2);
		return static_cast<RealNumber>(v3.m128_f32[0] + v3.m128_f32[1]);
	}

	template<>
	DUCK_FORCE_INLINE vec_t<float, 2>& vec_t<float, 2>::normalize()
	{
		RealNumber mag = magnitude();
		if (mag == 0) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}



	// vec2i
	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2> vec_t<int32_t, 2>::operator+(const vec_t<int32_t, 2>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_add_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2> vec_t<int32_t, 2>::operator-(const vec_t<int32_t, 2>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_sub_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2> vec_t<int32_t, 2>::operator*(const vec_t<int32_t, 2>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_mul_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2> vec_t<int32_t, 2>::operator/(const vec_t<int32_t, 2>& other) const
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
	DUCK_FORCE_INLINE vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator+=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		*v1 = _mm_add_epi32(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator-=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		*v1 = _mm_sub_epi32(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator*=(const vec_t<int32_t, 2>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		*v1 = _mm_mul_epi32(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2>& vec_t<int32_t, 2>::operator/=(const vec_t<int32_t, 2>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 0.0f, 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		return *this;
	}

	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<int32_t, 2>::square_magnitude() const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1 = _mm_mul_epi32(v1, v1);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1]);
	}

	template<>
	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<int32_t, 2>::dot(const vec_t<int32_t, 2>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;

		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;

		__m128i v3 = _mm_mul_epi32(v1, v2);
		return static_cast<RealNumber>(v3.m128i_i32[0] + v3.m128i_i32[1]);
	}

	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 2>& vec_t<int32_t, 2>::normalize()
	{
		RealNumber mag = magnitude();
		if (mag == 0) return *this;
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f };
		__m128 v2 = { mag, mag, mag, mag };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		return *this;
	}



	// vec3f
	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator+(const vec_t<float, 3>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_add_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator+(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_add_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator-(const vec_t<float, 3>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_sub_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator-(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_sub_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator*(const vec_t<float, 3>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_mul_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator*(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_mul_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator/(const vec_t<float, 3>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_div_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3> vec_t<float, 3>::operator/(const vec_t<float, 2>& other) const
	{
		const __m128* v1 = reinterpret_cast<const __m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		__m128 v3 = _mm_div_ps(*v1, *v2);
		return { v3.m128_f32[0], v3.m128_f32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator+=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_add_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator+=(const vec_t<float, 2>& other)
	{
		__m128 v1 = _mm_add_ps({ x, y, z, 0 }, { x, y, 0, 0 });
		x = v1.m128_f32[0];
		y = v1.m128_f32[1];
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator-=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_sub_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator-=(const vec_t<float, 2>& other)
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, 0 }, { x, y, 0, 0 });
		x = v1.m128_f32[0];
		y = v1.m128_f32[1];
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator*=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_mul_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator*=(const vec_t<float, 2>& other)
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, 0 }, { x, y, 0, 0 });
		x = v1.m128_f32[0];
		y = v1.m128_f32[1];
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator/=(const vec_t<float, 3>& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		const __m128* v2 = reinterpret_cast<const __m128*>(other.data);
		*v1 = _mm_div_ps(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::operator/=(const vec_t<float, 2>& other)
	{
		__m128 v1 = _mm_div_ps({ x, y, z, 0 }, { x, y, 0, 0 });
		x = v1.m128_f32[0];
		y = v1.m128_f32[1];
		return *this;
	}

	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<float, 3>::square_magnitude() const
	{
		__m128 v1 = { x, y, z, 0.0f };
		v1 = _mm_mul_ps(v1, v1);
		return static_cast<RealNumber>(v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2]);
	}

	template<>
	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<float, 3>::dot(const vec_t<float, 3>& other) const
	{
		__m128 v1 = { x, y, z, 0.0f };
		__m128 v2 = { other.x, other.y, other.z, 0.0f };
		__m128 v3 = _mm_mul_ps(v1, v2);
		return static_cast<RealNumber>(v3.m128_f32[0] + v3.m128_f32[1] + v3.m128_f32[2]);
	}

	template<>
	DUCK_FORCE_INLINE vec_t<float, 3>& vec_t<float, 3>::normalize()
	{
		RealNumber mag = magnitude();
		if (mag == 0) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}



	// vec3i
	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator+(const vec_t<int32_t, 3>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_add_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1], v3.m128i_i32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator+(const vec_t<int32_t, 2>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_add_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator-(const vec_t<int32_t, 3>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_sub_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1], v3.m128i_i32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator-(const vec_t<int32_t, 2>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_sub_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator*(const vec_t<int32_t, 3>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_mul_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1], v3.m128i_i32[2] };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator*(const vec_t<int32_t, 2>& other) const
	{
		const __m128i* v1 = reinterpret_cast<const __m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		__m128i v3 = _mm_mul_epi32(*v1, *v2);
		return { v3.m128i_i32[0], v3.m128i_i32[1], z };
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator/(const vec_t<int32_t, 3>& other) const
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
	DUCK_FORCE_INLINE vec_t<int32_t, 3> vec_t<int32_t, 3>::operator/(const vec_t<int32_t, 2>& other) const
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
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator+=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		*v1 = _mm_add_epi32(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator+=(const vec_t<int32_t, 2>& other)
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		__m128i v3 = _mm_add_epi32(v1, v2);
		x = v3.m128i_i32[0];
		y = v3.m128i_i32[1];
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator-=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		*v1 = _mm_sub_epi32(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator-=(const vec_t<int32_t, 2>& other)
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		__m128i v3 = _mm_sub_epi32(v1, v2);
		x = v3.m128i_i32[0];
		y = v3.m128i_i32[1];
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator*=(const vec_t<int32_t, 3>& other)
	{
		__m128i* v1 = reinterpret_cast<__m128i*>(data);
		const __m128i* v2 = reinterpret_cast<const __m128i*>(other.data);
		*v1 = _mm_mul_epi32(*v1, *v2);
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator*=(const vec_t<int32_t, 2>& other)
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v1.m128i_i32[0] = other.x;
		v1.m128i_i32[1] = other.y;

		__m128i v3 = _mm_mul_epi32(v1, v2);
		x = v3.m128i_i32[0];
		y = v3.m128i_i32[1];
		return *this;
	}

	template<>
	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator/=(const vec_t<int32_t, 3>& other)
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
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::operator/=(const vec_t<int32_t, 2>& other)
	{
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0.0f };
		__m128 v2 = { static_cast<float>(other.x), static_cast<float>(other.y), 0.0f, 0.0f };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		return *this;
	}

	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<int32_t, 3>::square_magnitude() const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;
		__m128i v3 = _mm_mul_epi32(v1, v1);
		return static_cast<RealNumber>(v1.m128i_i32[0] + v1.m128i_i32[1] + v1.m128i_i32[2]);
	}

	template<>
	template<>
	DUCK_FORCE_INLINE RealNumber vec_t<int32_t, 3>::dot(const vec_t<int32_t, 3>& other) const
	{
		__m128i v1 = {};
		v1.m128i_i32[0] = x;
		v1.m128i_i32[1] = y;
		v1.m128i_i32[2] = z;

		__m128i v2 = {};
		v2.m128i_i32[0] = other.x;
		v2.m128i_i32[1] = other.y;
		v2.m128i_i32[2] = other.z;

		__m128i v3 = _mm_mul_epi32(v1, v2);
		return static_cast<RealNumber>(v3.m128i_i32[0] + v3.m128i_i32[1] + v3.m128i_i32[2]);
	}

	template<>
	DUCK_FORCE_INLINE vec_t<int32_t, 3>& vec_t<int32_t, 3>::normalize()
	{
		RealNumber mag = magnitude();
		if (mag == 0) return *this;
		__m128 v1 = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 0.0f };
		__m128 v2 = { mag, mag, mag, mag };
		__m128 v3 = _mm_div_ps(v1, v2);
		x = static_cast<int32_t>(v3.m128_f32[0]);
		y = static_cast<int32_t>(v3.m128_f32[1]);
		z = static_cast<int32_t>(v3.m128_f32[2]);
		return *this;
	}

#endif
}