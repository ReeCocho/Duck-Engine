#pragma once

/**
 * @file vectors.hpp
 * @brief Duck math vectors.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <string>
#include <cmath>
#include <utilities/common.hpp>
#include "config.hpp"

#if DUCK_USE_SIMD
	#include <emmintrin.h>
	#include <smmintrin.h>
#endif

namespace dk
{
	/**
	 * @brief Generic N dimensional vector.
	 * @tparam Datatype used by the vector.
	 * @tparam Number of elements.
	 */
	template<typename T, size_t N>
	class alignas(16) vec_t
	{
	public:

		/**
		 * @brief Constructor.
		 */
		vec_t() = default;

		/**
		 * @brief Constructor.
		 * @param The value of every element in the vector.
		 */
		vec_t(T val)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] = val;
		}

		/**
		 * @brief Constructor.
		 * @param Values.
		 */
		vec_t(std::initializer_list<T> a_data)
		{
			dk_static_assert(a_data.size() <= N);

			size_t i = 0;
			for (auto e : a_data)
			{
				data[i] = e;
				++i;
			}
		}

		/**
		 * @param Copy constructor.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] = static_cast<T>(other.data[i]);
		}

		/**
		 * @brief Destructor.
		 */
		~vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const
		{
			std::string str = "(";

			for (size_t i = 0; i < N - 1; ++i)
				str += data[i] + ", ";

			str += data[N - 1] + ")";
			return str;
		}

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const
		{
			return N;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, N>& other) const
		{
			for (size_t i = 0; i < N; ++i)
				if (data[i] != other.data[i])
					return false;

			return true;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, N>& other) const
		{
			for (size_t i = 0; i < N; ++i)
				if (data[i] == other.data[i])
					return false;

			return true;
		}



		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, N> operator+(const vec_t<T2, N>& other) const
		{
			vec_t<T, N> new_vec = *this;

			for (size_t i = 0; i < N; ++i)
				new_vec.data[i] += static_cast<T>(other.data[i]);

			return new_vec;
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, N> operator-(const vec_t<T2, N>& other) const
		{
			vec_t<T, N> new_vec = *this;

			for (size_t i = 0; i < N; ++i)
				new_vec.data[i] -= static_cast<T>(other.data[i]);

			return new_vec;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, N> operator*(const vec_t<T2, N>& other) const
		{
			vec_t<T, N> new_vec = *this;

			for (size_t i = 0; i < N; ++i)
				new_vec.data[i] *= static_cast<T>(other.data[i]);

			return new_vec;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, N> operator/(const vec_t<T2, N>& other) const
		{
			vec_t<T, N> new_vec = *this;

			for (size_t i = 0; i < N; ++i)
				new_vec.data[i] /= static_cast<T>(other.data[i]);

			return new_vec;
		}



		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, N>& operator+=(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] += static_cast<T>(other.data[i]);

			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, N>& operator-=(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] -= static_cast<T>(other.data[i]);

			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, N>& operator*=(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] *= static_cast<T>(other.data[i]);

			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, N>& operator/=(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] /= static_cast<T>(other.data[i]);

			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, N>& operator=(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] = static_cast<T>(other.data[i]);

			return *this;
		}

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		RealNumber square_magnitude() const
		{
			RealNumber sqr_total = 0;

			for (size_t i = 0; i < N; ++i)
				sqr_total += static_cast<RealNumber>(data[i] * data[i]);

			return sqr_total;
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		RealNumber dot(const vec_t<T2, N>& other) const
		{
			RealNumber mult_total = 0;

			for (size_t i = 0; i < N; ++i)
				mult_total += (static_cast<RealNumber>(data[i]) * static_cast<RealNumber>(other.data[i]));

			return mult_total;
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		vec_t<T, N>& normalize()
		{
			RealNumber mag = magnitude();
			if (mag == 0) return *this;
			
			for (size_t i = 0; i < N; ++i)
				m_data[i] /= mag;

			return *this;
		}

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		vec_t<T, N> normalized() const
		{
			RealNumber mag = magnitude();
			if (mag == 0) return vec_t<T, N>(0);

			vec_t<T, N> new_vec = *this;

			for (size_t i = 0; i < N; ++i)
				new_vec[i] /= mag;

			return new_vec;
		}

		/** Vectors data. */
		T data[N];
	};



	/**
	 * @brief A 2 dimensional vector.
	 * @tparam Datatype used by the vector.
	 */
	template<typename T>
	class alignas(16) vec_t<T, 2>
	{
	public:

		/**
		 * @brief Constructor.
		 */
		vec_t() : x(0), y(0) {}

		/**
		 * @brief Constructor.
		 * @param The value of every element.
		 */
		vec_t(T x_a) : x(x_a), y(x_a) {}

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 */
		vec_t(T x_a, T y_a) : x(x_a), y(y_a) {}

		/**
		 * @brief Copy constructor.
		 * @param Other vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, 2>& other) : 
			x(static_cast<T>(other.x)), 
			y(static_cast<T>(other.y)) {}

		/**
		 * @brief Destructor.
		 */
		~vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const
		{
			return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
		}

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const
		{
			return 2;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, 2>& other) const
		{
			return x == other.x && y == other.y;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, 2>& other) const
		{
			return x != other.x || y != other.y;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, 2> operator+(const vec_t<T2, 2>& other) const
		{
			return
			{
				x + static_cast<T>(other.x),
				y + static_cast<T>(other.y)
			};
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, 2> operator-(const vec_t<T2, 2>& other) const
		{
			return
			{
				x - static_cast<T>(other.x),
				y - static_cast<T>(other.y)
			};
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, 2> operator*(const vec_t<T2, 2>& other) const
		{
			return
			{
				x * static_cast<T>(other.x),
				y * static_cast<T>(other.y)
			};
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, 2> operator/(const vec_t<T2, 2>& other) const
		{
			return
			{
				x / static_cast<T>(other.x),
				y / static_cast<T>(other.y)
			};
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 2>& operator+=(const vec_t<T2, 2>& other)
		{
			x += static_cast<T>(other.x);
			y += static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 2>& operator-=(const vec_t<T2, 2>& other)
		{
			x -= static_cast<T>(other.x);
			y -= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 2>& operator*=(const vec_t<T2, 2>& other)
		{
			x *= static_cast<T>(other.x);
			y *= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 2>& operator/=(const vec_t<T2, 2>& other)
		{
			x /= static_cast<T>(other.x);
			y /= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 2>& operator=(const vec_t<T2, 2>& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		RealNumber square_magnitude() const
		{
			return static_cast<RealNumber>((x*x) + (y*y));
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		RealNumber dot(const vec_t<T2, 2>& other) const
		{
			return static_cast<RealNumber>((x * other.x) + (y * other.y));
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		vec_t<T, 2>& normalize()
		{
			RealNumber mag = magnitude();
			if (mag == 0) return;
			x = static_cast<T>(static_cast<RealNumber>(x) / mag);
			y = static_cast<T>(static_cast<RealNumber>(y) / mag);
			return *this;
		}

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		vec_t<T, 2> normalized() const
		{
			RealNumber mag = magnitude();
			if (mag == 0) return vec_t<T, N>(0);
			vec_t<T, N> new_vec = *this;
			new_vec.x /= static_cast<T>(mag);
			new_vec.y /= static_cast<T>(mag);
			return new_vec;
		}

		/** Vector data. */
		union
		{
			struct { T x, y; };
			struct { T u, v; };
			T data[2];
		};
	};

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

#endif



	/**
	 * @brief A 3 dimensional vector.
	 * @tparam Datatype used by the vector.
	 */
	template<typename T>
	class alignas(16) vec_t<T, 3>
	{
	public:

		/**
		 * @brief Constructor.
		 */
		vec_t() : x(0), y(0), z(0) {}

		/**
		 * @brief Constructor.
		 * @param Value of every element.
		 */
		vec_t(T val) : x(val), y(val), z(val) {}

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 * @param Z value.
		 */
		vec_t(T x_a, T y_a, T z_a) : x(x_a), y(y_a), z(z_a) {}

		/**
		 * @brief Copy constructor.
		 * @param Other 2D vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, 2>& other) : 
			x(static_cast<T>(other.x)), 
			y(static_cast<T>(other.y)), 
			z(0) {}

		/**
		 * @brief Copy constructor.
		 * @param Other 3D vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, 3>& other) : 
			x(static_cast<T>(other.x)), 
			y(static_cast<T>(other.y)), 
			z(static_cast<T>(other.z)) {}

		/**
		 * @brief Destructor.
		 */
		~vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const
		{
			return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
		}

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const
		{
			return 3;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, 3>& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, 2>& other) const
		{
			return x == other.x && y == other.y && z == 0;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, 3>& other) const
		{
			return x != other.x || y != other.y || z != other.z;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, 2>& other) const
		{
			return x != other.x || y != other.y || z != 0;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, 3> operator+(const vec_t<T2, 3>& other) const
		{
			return
			{
				x + static_cast<T>(other.x),
				y + static_cast<T>(other.y),
				z + static_cast<T>(other.z)
			};
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, 3> operator+(const vec_t<T2, 2>& other) const
		{
			return
			{
				x + static_cast<T>(other.x),
				y + static_cast<T>(other.y),
				z
			};
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, 3> operator-(const vec_t<T2, 3>& other) const
		{
			return
			{
				x - static_cast<T>(other.x),
				y - static_cast<T>(other.y),
				z - static_cast<T>(other.z)
			};
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, 3> operator-(const vec_t<T2, 2>& other) const
		{
			return
			{
				x - static_cast<T>(other.x),
				y - static_cast<T>(other.y),
				z
			};
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, 3> operator*(const vec_t<T2, 3>& other) const
		{
			return
			{
				x * static_cast<T>(other.x),
				y * static_cast<T>(other.y),
				z * static_cast<T>(other.z)
			};
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, 3> operator*(const vec_t<T2, 2>& other) const
		{
			return
			{
				x * static_cast<T>(other.x),
				y * static_cast<T>(other.y),
				z
			};
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, 3> operator/(const vec_t<T2, 3>& other) const
		{
			return
			{
				x / static_cast<T>(other.x),
				y / static_cast<T>(other.y),
				z / static_cast<T>(other.z)
			};
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, 3> operator/(const vec_t<T2, 2>& other) const
		{
			return
			{
				x / static_cast<T>(other.x),
				y / static_cast<T>(other.y),
				z
			};
		}



		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator+=(const vec_t<T2, 3>& other)
		{
			x += static_cast<T>(other.x);
			y += static_cast<T>(other.y);
			z += static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator+=(const vec_t<T2, 2>& other)
		{
			x += static_cast<T>(other.x);
			y += static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator-=(const vec_t<T2, 3>& other)
		{
			x -= static_cast<T>(other.x);
			y -= static_cast<T>(other.y);
			z -= static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator-=(const vec_t<T2, 2>& other)
		{
			x -= static_cast<T>(other.x);
			y -= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator*=(const vec_t<T2, 3>& other)
		{
			x *= static_cast<T>(other.x);
			y *= static_cast<T>(other.y);
			z *= static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator*=(const vec_t<T2, 2>& other)
		{
			x *= static_cast<T>(other.x);
			y *= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator/=(const vec_t<T2, 3>& other)
		{
			x /= static_cast<T>(other.x);
			y /= static_cast<T>(other.y);
			z /= static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator/=(const vec_t<T2, 2>& other)
		{
			x /= static_cast<T>(other.x);
			y /= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator=(const vec_t<T2, 3>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 3>& operator=(const vec_t<T2, 2>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = 0;
			return *this;
		}

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		RealNumber square_magnitude() const
		{
			return static_cast<RealNumber>((x*x) + (y*y) + (z*z));
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		RealNumber dot(const vec_t<T2, 3>& other) const
		{
			return static_cast<RealNumber>((x*other.x) + (y*other.y) + (z*other.z));
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		vec_t<T, 3>& normalize()
		{
			RealNumber mag = magnitude();
			if (mag == 0) return;
			x = static_cast<T>(static_cast<RealNumber>(x) / mag);
			y = static_cast<T>(static_cast<RealNumber>(y) / mag);
			z = static_cast<T>(static_cast<RealNumber>(z) / mag);
			return *this;
		}

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		vec_t<T, 3> normalized() const
		{
			RealNumber mag = magnitude();
			if (mag == 0) return vec_t<T, N>(0);
			vec_t<T, N> new_vec = *this;
			new_vec.x /= static_cast<T>(mag);
			new_vec.y /= static_cast<T>(mag);
			new_vec.z /= static_cast<T>(mag);
			return new_vec;
		}

		/** Vector data. */
		union
		{
			struct { T x, y, z; };
			struct { T r, g, b; };
			struct { T u, v, w; };
			T data[3];
		};
	};

#if DUCK_USE_SIMD

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

#endif



	/**
	 * @brief A 4 dimensional vector.
	 * @tparam Datatype used by the vector.
	 */
	template<typename T>
	class alignas(16) vec_t<T, 4>
	{
	public:

		/**
		 * @brief Constructor.
		 */
		vec_t() : x(0), y(0), z(0), w(0) {}

		/**
		 * @brief Constructor.
		 * @param Value of every element.
		 */
		vec_t(T val) : x(val), y(val), z(val), w(val) {}

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 * @param Z value.
		 * @param W value.
		 */
		vec_t(T x_a, T y_a, T z_a, T w_a) : x(x_a), y(y_a), z(z_a), w(w_a) {}

		/**
		 * @brief Copy constructor.
		 * @param Other 2D vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, 2>& other) : 
			x(static_cast<T>(other.x)), 
			y(static_cast<T>(other.y)), 
			z(0),
			w(0) {}

		/**
		 * @brief Copy constructor.
		 * @param Other 3D vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, 3>& other) : 
			x(static_cast<T>(other.x)), 
			y(static_cast<T>(other.y)), 
			z(static_cast<T>(other.z)),
			w(0) {}

		/**
		 * @brief Copy constructor.
		 * @param Other 4D vector.
		 */
		template<typename T2>
		vec_t(const vec_t<T2, 4>& other) : 
			x(static_cast<T>(other.x)), 
			y(static_cast<T>(other.y)), 
			z(static_cast<T>(other.z)),
			w(static_cast<T>(other.w)) {}

		/**
		 * @brief Destructor.
		 */
		~vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const
		{
			return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
		}

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const
		{
			return 4;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, 4>& other) const
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, 3>& other) const
		{
			return x == other.x && y == other.y && z == other.z && w == 0;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const vec_t<T, 2>& other) const
		{
			return x == other.x && y == other.y && z == 0 && w == 0;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, 4>& other) const
		{
			return x != other.x || y != other.y || z != other.z || w != other.w;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, 3>& other) const
		{
			return x != other.x || y != other.y || z != other.z || w != 0;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const vec_t<T, 2>& other) const
		{
			return x != other.x || y != other.y || z != 0 || w != 0;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, 4> operator+(const vec_t<T2, 4>& other) const
		{
			return
			{
				x + static_cast<T>(other.x),
				y + static_cast<T>(other.y),
				z + static_cast<T>(other.z),
				w + static_cast<T>(other.w)
			};
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, 4> operator+(const vec_t<T2, 3>& other) const
		{
			return
			{
				x + static_cast<T>(other.x),
				y + static_cast<T>(other.y),
				z + static_cast<T>(other.z),
				w
			};
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		template<typename T2>
		vec_t<T, 4> operator+(const vec_t<T2, 2>& other) const
		{
			return
			{
				x + static_cast<T>(other.x),
				y + static_cast<T>(other.y),
				z,
				w
			};
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, 4> operator-(const vec_t<T2, 4>& other) const
		{
			return
			{
				x - static_cast<T>(other.x),
				y - static_cast<T>(other.y),
				z - static_cast<T>(other.z),
				w - static_cast<T>(other.w)
			};
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, 4> operator-(const vec_t<T2, 3>& other) const
		{
			return
			{
				x - static_cast<T>(other.x),
				y - static_cast<T>(other.y),
				z - static_cast<T>(other.z),
				w
			};
		}

		/**
		 * @brief Subtration operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		template<typename T2>
		vec_t<T, 4> operator-(const vec_t<T2, 2>& other) const
		{
			return
			{
				x - static_cast<T>(other.x),
				y - static_cast<T>(other.y),
				z,
				w
			};
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, 4> operator*(const vec_t<T2, 4>& other) const
		{
			return
			{
				x * static_cast<T>(other.x),
				y * static_cast<T>(other.y),
				z * static_cast<T>(other.z),
				w * static_cast<T>(other.w)
			};
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, 4> operator*(const vec_t<T2, 3>& other) const
		{
			return
			{
				x * static_cast<T>(other.x),
				y * static_cast<T>(other.y),
				z * static_cast<T>(other.z),
				w
			};
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		template<typename T2>
		vec_t<T, 4> operator*(const vec_t<T2, 2>& other) const
		{
			return
			{
				x * static_cast<T>(other.x),
				y * static_cast<T>(other.y),
				z,
				w
			};
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, 4> operator/(const vec_t<T2, 4>& other) const
		{
			return
			{
				x / static_cast<T>(other.x),
				y / static_cast<T>(other.y),
				z / static_cast<T>(other.z),
				w / static_cast<T>(other.w)
			};
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, 4> operator/(const vec_t<T2, 3>& other) const
		{
			return
			{
				x / static_cast<T>(other.x),
				y / static_cast<T>(other.y),
				z / static_cast<T>(other.z),
				w
			};
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		template<typename T2>
		vec_t<T, 4> operator/(const vec_t<T2, 2>& other) const
		{
			return
			{
				x / static_cast<T>(other.x),
				y / static_cast<T>(other.y),
				z,
				w
			};
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator+=(const vec_t<T2, 4>& other)
		{
			x += static_cast<T>(other.x);
			y += static_cast<T>(other.y);
			z += static_cast<T>(other.z);
			w += static_cast<T>(other.w);
			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator+=(const vec_t<T2, 3>& other)
		{
			x += static_cast<T>(other.x);
			y += static_cast<T>(other.y);
			z += static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator+=(const vec_t<T2, 2>& other)
		{
			x += static_cast<T>(other.x);
			y += static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator-=(const vec_t<T2, 4>& other)
		{
			x -= static_cast<T>(other.x);
			y -= static_cast<T>(other.y);
			z -= static_cast<T>(other.z);
			w -= static_cast<T>(other.w);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator-=(const vec_t<T2, 3>& other)
		{
			x -= static_cast<T>(other.x);
			y -= static_cast<T>(other.y);
			z -= static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator-=(const vec_t<T2, 2>& other)
		{
			x -= static_cast<T>(other.x);
			y -= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator*=(const vec_t<T2, 4>& other)
		{
			x *= static_cast<T>(other.x);
			y *= static_cast<T>(other.y);
			z *= static_cast<T>(other.z);
			w *= static_cast<T>(other.w);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator*=(const vec_t<T2, 3>& other)
		{
			x *= static_cast<T>(other.x);
			y *= static_cast<T>(other.y);
			z *= static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator*=(const vec_t<T2, 2>& other)
		{
			x *= static_cast<T>(other.x);
			y *= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator/=(const vec_t<T2, 4>& other)
		{
			x /= static_cast<T>(other.x);
			y /= static_cast<T>(other.y);
			z /= static_cast<T>(other.z);
			w /= static_cast<T>(other.w);
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator/=(const vec_t<T2, 3>& other)
		{
			x /= static_cast<T>(other.x);
			y /= static_cast<T>(other.y);
			z /= static_cast<T>(other.z);
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator/=(const vec_t<T2, 2>& other)
		{
			x /= static_cast<T>(other.x);
			y /= static_cast<T>(other.y);
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator=(const vec_t<T2, 4>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = static_cast<T>(other.z);
			w = static_cast<T>(other.w);
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator=(const vec_t<T2, 3>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = static_cast<T>(other.z);
			w = 0;
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other vector.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		vec_t<T, 4>& operator=(const vec_t<T2, 2>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = 0;
			w = 0;
			return *this;
		}

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		RealNumber square_magnitude() const
		{
			return static_cast<RealNumber>((x*x) + (y*y) + (z*z) + (w*w));
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		RealNumber dot(const vec_t<T2, 4>& other) const
		{
			return static_cast<RealNumber>((x*other.x) + (y*other.y) + (z*other.z) + (w*other.w));
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		vec_t<T, 4>& normalize()
		{
			RealNumber mag = magnitude();
			if (mag == 0) return;
			x = static_cast<T>(static_cast<RealNumber>(x) / mag);
			y = static_cast<T>(static_cast<RealNumber>(y) / mag);
			z = static_cast<T>(static_cast<RealNumber>(z) / mag);
			w = static_cast<T>(static_cast<RealNumber>(w) / mag);
			return *this;
		}

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		vec_t<T, 4> normalized() const
		{
			RealNumber mag = magnitude();
			if (mag == 0) return vec_t<T, N>(0);
			vec_t<T, N> new_vec = *this;
			new_vec.x /= static_cast<T>(mag);
			new_vec.y /= static_cast<T>(mag);
			new_vec.z /= static_cast<T>(mag);
			new_vec.w /= static_cast<T>(mag);
			return new_vec;
		}

		/** Vector data. */
		union
		{
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			T data[4];
		};
	};

#if DUCK_USE_SIMD

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

	/** Standard 4D vector. */
	using vec4 = vec_t<RealNumber, 4>;

	/** Floating point 4D vector. */
	using vec4f = vec_t<float, 4>;

	/** Double precision floating point 4D vector. */
	using vec4d = vec_t<double, 4>;

	/** Signed integer 4D vector. */
	using vec4i = vec_t<int32_t, 4>;



	/** Standard 3D vector. */
	using vec3 = vec_t<RealNumber, 3>;

	/** Floating point 3D vector. */
	using vec3f = vec_t<float, 3>;

	/** Double precision floating point 2D vector. */
	using vec3d = vec_t<double, 3>;

	/** Signed integer 2D vector. */
	using vec3i = vec_t<int32_t, 3>;



	/** Standard 2D vector. */
	using vec2 = vec_t<RealNumber, 2>;

	/** Floating point 2D vector. */
	using vec2f = vec_t<float, 2>;

	/** Double precision floating point 2D vector. */
	using vec2d = vec_t<double, 2>;

	/** Signed integer 2D vector. */
	using vec2i = vec_t<int32_t, 2>;
}