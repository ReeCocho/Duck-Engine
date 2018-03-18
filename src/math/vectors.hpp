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
	 * @tparam Number of elements.
	 */
	template<size_t N>
	class alignas(16) Vec_t
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Vec_t() = default;

		/**
		 * @brief Constructor.
		 * @param The value of every element in the vector.
		 */
		Vec_t(float val)
		{
			for (size_t i = 0; i < N; ++i) data[i] = val;
		}

		/**
		 * @brief Constructor.
		 * @param Values.
		 */
		Vec_t(std::initializer_list<float> a_data)
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
		 * @param Other vector.
		 */
		Vec_t(const Vec_t<N>& other)
		{
			for (size_t i = 0; i < N; ++i) data[i] = other.data[i];
		}

		/**
		 * @brief Destructor.
		 */
		~Vec_t() = default;



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
		bool operator==(const Vec_t<N>& other) const
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
		bool operator!=(const Vec_t<N>& other) const
		{
			for (size_t i = 0; i < N; ++i)
				if (data[i] == other.data[i])
					return false;

			return true;
		}



		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<N> operator+(const Vec_t<N>& other) const
		{
			Vec_t<N> new_vec = *this;
			for (size_t i = 0; i < N; ++i) new_vec.data[i] += other.data[i];
			return new_vec;
		}

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<N> operator-(const Vec_t<N>& other) const
		{
			Vec_t<N> new_vec = *this;
			for (size_t i = 0; i < N; ++i) new_vec.data[i] -= other.data[i];
			return new_vec;
		}

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<N> operator*(const Vec_t<N>& other) const
		{
			Vec_t<N> new_vec = *this;
			for (size_t i = 0; i < N; ++i) new_vec.data[i] *= other.data[i];
			return new_vec;
		}

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<N> operator/(const Vec_t<N>& other) const
		{
			Vec_t<N> new_vec = *this;
			for (size_t i = 0; i < N; ++i) new_vec.data[i] /= other.data[i];
			return new_vec;
		}



		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<N>& operator+=(const Vec_t<N>& other)
		{
			for (size_t i = 0; i < N; ++i) data[i] += other.data[i];
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<N>& operator-=(const Vec_t<N>& other)
		{
			for (size_t i = 0; i < N; ++i) data[i] -= other.data[i];
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<N>& operator*=(const Vec_t<N>& other)
		{
			for (size_t i = 0; i < N; ++i) data[i] *= other.data[i];
			return *this;
		}

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		template<typename T2>
		Vec_t<N>& operator/=(const Vec_t<N>& other)
		{
			for (size_t i = 0; i < N; ++i) data[i] /= other.data[i];
			return *this;
		}

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<N>& operator=(const Vec_t<N>& other)
		{
			for (size_t i = 0; i < N; ++i) data[i] = other.data[i];
			return *this;
		}

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		float square_magnitude() const
		{
			float sqr_total = 0;
			for (size_t i = 0; i < N; ++i) sqr_total += data[i] * data[i];
			return sqr_total;
		}

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		float magnitude() const
		{
			return static_cast<float>(std::sqrt(square_magnitude()));
		}

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		float dot(const Vec_t<N>& other) const
		{
			float mult_total = 0;
			for (size_t i = 0; i < N; ++i) mult_total += data[i] * other.data[i];
			return mult_total;
		}

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		Vec_t<N>& normalize()
		{
			float mag = magnitude();
			if (mag == 0.0f) return *this;
			for (size_t i = 0; i < N; ++i) m_data[i] /= mag;
			return *this;
		}

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		Vec_t<N> normalized() const
		{
			float mag = magnitude();
			if (mag == 0.0f) return Vec_t<N>(0);
			Vec_t<N> new_vec = *this;
			for (size_t i = 0; i < N; ++i) new_vec[i] /= mag;
			return new_vec;
		}

		/** Vectors data. */
		float data[N];
	};



	/**
	 * @brief A 2 dimensional vector.
	 */
	template<>
	class alignas(16) Vec_t<2>
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Vec_t();

		/**
		 * @brief Constructor.
		 * @param The value of every element.
		 */
		Vec_t(float x_a);

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 */
		Vec_t(float x_a, float y_a);

		/**
		 * @brief Copy constructor.
		 * @param Other vector.
		 */
		Vec_t(const Vec_t<2>& other);

		/**
		 * @brief Destructor.
		 */
		~Vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const;

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const;

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const Vec_t<2>& other) const;

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const Vec_t<2>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<2> operator+(const Vec_t<2>& other) const;

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<2> operator-(const Vec_t<2>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<2> operator*(const Vec_t<2>& other) const;

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<2> operator/(const Vec_t<2>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<2>& operator+=(const Vec_t<2>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<2>& operator-=(const Vec_t<2>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<2>& operator*=(const Vec_t<2>& other);

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<2>& operator/=(const Vec_t<2>& other);

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<2>& operator=(const Vec_t<2>& other);

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		float square_magnitude() const;

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		float magnitude() const;

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		float dot(const Vec_t<2>& other) const;

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		Vec_t<2>& normalize();

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		Vec_t<2> normalized() const;

		/** Vector data. */
		union
		{
			struct { float x, y; };
			struct { float u, v; };
			float data[2];
		};
	};



	/**
	 * @brief A 3 dimensional vector.
	 * @tparam Datatype used by the vector.
	 */
	template<>
	class alignas(16) Vec_t<3>
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Vec_t();

		/**
		 * @brief Constructor.
		 * @param Value of every element.
		 */
		Vec_t(float val);

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 * @param Z value.
		 */
		Vec_t(float x_a, float y_a, float z_a);

		/**
		 * @brief Copy constructor.
		 * @param Other 2D vector.
		 */
		Vec_t(const Vec_t<2>& other);

		/**
		 * @brief Copy constructor.
		 * @param Other 3D vector.
		 */
		Vec_t(const Vec_t<3>& other);

		/**
		 * @brief Destructor.
		 */
		~Vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const;

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const;

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const Vec_t<3>& other) const;

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const Vec_t<2>& other) const;

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const Vec_t<3>& other) const;

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const Vec_t<2>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<3> operator+(const Vec_t<3>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<3> operator+(const Vec_t<2>& other) const;

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<3> operator-(const Vec_t<3>& other) const;

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<3> operator-(const Vec_t<2>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<3> operator*(const Vec_t<3>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<3> operator*(const Vec_t<2>& other) const;

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<3> operator/(const Vec_t<3>& other) const;

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<3> operator/(const Vec_t<2>& other) const;



		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator+=(const Vec_t<3>& other);

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator+=(const Vec_t<2>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator-=(const Vec_t<3>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator-=(const Vec_t<2>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator*=(const Vec_t<3>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator*=(const Vec_t<2>& other);

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator/=(const Vec_t<3>& other);

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator/=(const Vec_t<2>& other);

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator=(const Vec_t<3>& other);

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<3>& operator=(const Vec_t<2>& other);

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		float square_magnitude() const;

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		float magnitude() const;

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		float dot(const Vec_t<3>& other) const;

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		Vec_t<3>& normalize();

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		Vec_t<3> normalized() const;

		/** Vector data. */
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
			struct { float u, v, w; };
			float data[3];
		};
	};



	/**
	 * @brief A 4 dimensional vector.
	 */
	template<>
	class alignas(16) Vec_t<4>
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Vec_t();

		/**
		 * @brief Constructor.
		 * @param Value of every element.
		 */
		Vec_t(float val);

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 * @param Z value.
		 * @param W value.
		 */
		Vec_t(float x_a, float y_a, float z_a, float w_a);

		/**
		 * @brief Copy constructor.
		 * @param Other 2D vector.
		 */
		Vec_t(const Vec_t<2>& other);

		/**
		 * @brief Copy constructor.
		 * @param Other 3D vector.
		 */
		Vec_t(const Vec_t<3>& other);

		/**
		 * @brief Copy constructor.
		 * @param Other 4D vector.
		 */
		Vec_t(const Vec_t<4>& other);

		/**
		 * @brief Destructor.
		 */
		~Vec_t() = default;



		/**
		 * @brief Turn the vector into a string.
		 * @return Vector as a string.
		 */
		std::string to_string() const;

		/**
		 * @brief Get the number of elements in the vector.
		 * @return Number of elements in the vector.
		 */
		constexpr size_t size() const;

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const Vec_t<4>& other) const;

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const Vec_t<3>& other) const;

		/**
		 * @brief Equivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are equal.
		 */
		bool operator==(const Vec_t<2>& other) const;

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const Vec_t<4>& other) const;

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const Vec_t<3>& other) const;

		/**
		 * @brief Unequivelence operator.
		 * @param Other vector.
		 * @return If this vector and the other vector are not equal.
		 */
		bool operator!=(const Vec_t<2>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<4> operator+(const Vec_t<4>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<4> operator+(const Vec_t<3>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return Vector with summed values.
		 */
		Vec_t<4> operator+(const Vec_t<2>& other) const;

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<4> operator-(const Vec_t<4>& other) const;

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<4> operator-(const Vec_t<3>& other) const;

		/**
		 * @brief Subtration operator.
		 * @param Other vector.
		 * @return Vector with difference values.
		 */
		Vec_t<4> operator-(const Vec_t<2>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<4> operator*(const Vec_t<4>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<4> operator*(const Vec_t<3>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of other vector.
		 * @param Other vector.
		 * @return Vector with product values.
		 */
		Vec_t<4> operator*(const Vec_t<2>& other) const;

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<4> operator/(const Vec_t<4>& other) const;

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<4> operator/(const Vec_t<3>& other) const;

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return Vector with quotient values.
		 */
		Vec_t<4> operator/(const Vec_t<2>& other) const;

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator+=(const Vec_t<4>& other);

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator+=(const Vec_t<3>& other);

		/**
		 * @brief Addition operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator+=(const Vec_t<2>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator-=(const Vec_t<4>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator-=(const Vec_t<3>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator-=(const Vec_t<2>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator*=(const Vec_t<4>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator*=(const Vec_t<3>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator*=(const Vec_t<2>& other);

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator/=(const Vec_t<4>& other);

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator/=(const Vec_t<3>& other);

		/**
		 * @brief Division operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator/=(const Vec_t<2>& other);

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator=(const Vec_t<4>& other);

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator=(const Vec_t<3>& other);

		/**
		 * @brief Assignment operator.
		 * @param Other vector.
		 * @return This vector.
		 */
		Vec_t<4>& operator=(const Vec_t<2>& other);

		/**
		 * @brief Get the square magnitude of the vector.
		 * @return Square magnitude of the vector.
		 */
		float square_magnitude() const;

		/**
		 * @brief Get the magnitude of the vector.
		 * @return Magnitude of the vector.
		 */
		float magnitude() const;

		/**
		 * @brief Get the dot product between this vector and another.
		 * @param Other vector.
		 * @return Result of the dot product.
		 */
		float dot(const Vec_t<4>& other) const;

		/**
		 * @brief Normalize the vector.
		 * @return This vector.
		 */
		Vec_t<4>& normalize();

		/**
		 * @brief Create a normalized version of this vector.
		 * @return A vector with the same direction as this, but with a magnitude of 1.
		 * @note The magnitude might be 0 if the original vectors magnitude was 0.
		 */
		Vec_t<4> normalized() const;

		/** Vector data. */
		union
		{
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			float data[4];
		};
	};



	/** Standard 4D vector. */
	using Vec4 = Vec_t<4>;

	/** Standard 3D vector. */
	using Vec3 = Vec_t<3>;

	/** Standard 2D vector. */
	using Vec2 = Vec_t<2>;
}