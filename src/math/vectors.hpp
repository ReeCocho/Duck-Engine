#pragma once

/**
 * @file vectors.hpp
 * @brief Duck Math vectors.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <cmath>
#include <utilities\common.hpp>
#include "config.hpp"

#if DUCK_USE_SIMD
	#include <emmintrin.h>
	#include <smmintrin.h>
#endif

namespace dk
{
	/**
	 * @class vec_t
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
			dk_static_assert(a_data.size <= N);

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
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		DUCK_FORCE_INLINE bool operator==(const vec_t<T, N>& other) const
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
		DUCK_FORCE_INLINE bool operator!=(const vec_t<T, N>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, N> operator+(const vec_t<T2, N>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, N> operator-(const vec_t<T2, N>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, N> operator*(const vec_t<T2, N>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, N> operator/(const vec_t<T2, N>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, N>& operator+=(const vec_t<T2, N>& other)
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
		DUCK_FORCE_INLINE vec_t<T, N>& operator-=(const vec_t<T2, N>& other)
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
		DUCK_FORCE_INLINE vec_t<T, N>& operator*=(const vec_t<T2, N>& other)
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
		DUCK_FORCE_INLINE vec_t<T, N>& operator/=(const vec_t<T2, N>& other)
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
		DUCK_FORCE_INLINE vec_t<T, N>& operator=(const vec_t<T2, N>& other)
		{
			for (size_t i = 0; i < N; ++i)
				data[i] = static_cast<T>(other.data[i]);

			return *this;
		}

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		DUCK_FORCE_INLINE RealNumber square_magnitude() const
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
		DUCK_FORCE_INLINE RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		DUCK_FORCE_INLINE RealNumber dot(const vec_t<T2, N>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, N>& normalize()
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
		DUCK_FORCE_INLINE vec_t<T, N> normalized() const
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
	 * @class vec_t<T, 2>
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
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		DUCK_FORCE_INLINE bool operator==(const vec_t<T, 2>& other) const
		{
			return x == other.x && y == other.y;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		DUCK_FORCE_INLINE bool operator!=(const vec_t<T, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 2> operator+(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 2> operator-(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 2> operator*(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 2> operator/(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 2>& operator+=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 2>& operator-=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 2>& operator*=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 2>& operator/=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 2>& operator=(const vec_t<T2, 2>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			return *this;
		}



		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		DUCK_FORCE_INLINE RealNumber square_magnitude() const
		{
			return static_cast<RealNumber>((x*x) + (y*y));
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		DUCK_FORCE_INLINE RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		DUCK_FORCE_INLINE RealNumber dot(const vec_t<T2, 2>& other) const
		{
			return static_cast<RealNumber>((x * other.x) + (y * other.y));
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		DUCK_FORCE_INLINE vec_t<T, 2>& normalize()
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
		DUCK_FORCE_INLINE vec_t<T, 2> normalized() const
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
			T data[2];
		};
	};



	/**
	 * @class vec_t<T, 3>
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
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		DUCK_FORCE_INLINE bool operator==(const vec_t<T, 3>& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		DUCK_FORCE_INLINE bool operator==(const vec_t<T, 2>& other) const
		{
			return x == other.x && y == other.y && z == 0;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		DUCK_FORCE_INLINE bool operator!=(const vec_t<T, 3>& other) const
		{
			return x != other.x || y != other.y || z != other.z;
		}

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		DUCK_FORCE_INLINE bool operator!=(const vec_t<T, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator+(const vec_t<T2, 3>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator+(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator-(const vec_t<T2, 3>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator-(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator*(const vec_t<T2, 3>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator*(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator/(const vec_t<T2, 3>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3> operator/(const vec_t<T2, 2>& other) const
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator+=(const vec_t<T2, 3>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator+=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator-=(const vec_t<T2, 3>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator-=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator*=(const vec_t<T2, 3>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator*=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator/=(const vec_t<T2, 3>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator/=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator=(const vec_t<T2, 3>& other)
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
		DUCK_FORCE_INLINE vec_t<T, 3>& operator=(const vec_t<T2, 2>& other)
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
		DUCK_FORCE_INLINE RealNumber square_magnitude() const
		{
			return static_cast<RealNumber>((x*x) + (y*y) + (z*z));
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		DUCK_FORCE_INLINE RealNumber magnitude() const
		{
			return static_cast<RealNumber>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		template<typename T2>
		DUCK_FORCE_INLINE RealNumber dot(const vec_t<T2, 3>& other) const
		{
			return static_cast<RealNumber>((x*other.x) + (y*other.y) + (z*other.z));
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		DUCK_FORCE_INLINE vec_t<T, 3>& normalize()
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
		DUCK_FORCE_INLINE vec_t<T, 3> normalized() const
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
			T data[3];
		};
	};



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