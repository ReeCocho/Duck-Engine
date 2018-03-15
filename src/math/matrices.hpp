#pragma once

/**
 * @file matrices.hpp
 * @brief Duck math matrices.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vectors.hpp"

namespace dk
{
	/**
	 * @brief A row in a matrix.
	 * @tparam Type of data in the row.
	 * @tparam Number of elements in the row.
	 */
	template<typename T, size_t N>
	class Row
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Pointer to first element in the row.
		 */
		Row(T* data) : m_data(data) {}

		/**
		 * @brief Access operator.
		 * @param Index in the row.
		 * @return Element at the index.
		 */
		T& operator[](size_t index)
		{
			dk_assert(index < N);
			return m_data[index];
		}

	private:

		/** Pointer to first element in the row. */
		T* m_data = nullptr;
	};

	/**
	 * @brief M by N matrix.
	 * @tparam Type used by the matrix.
	 * @tparam M dimension.
	 * @tparam N dimension.
	 */
	template<typename T, size_t M, size_t N>
	class alignas(16) Mat_t
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Mat_t() = default;

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		Mat_t(T val)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = val;
		}
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(std::initializer_list<T> values)
		{
			dk_assert(values.size() == N * M);

			size_t i = 0;
			for (auto value : values)
			{
				data[i / M][i%N] = value;
				++i;
			}
		}

		/**
		 * @brief Copy constructor.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 */
		template<typename T2>
		Mat_t(const Mat_t<T2, M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = static_cast<T>(other.data[i][j]);
		}

		/**
		 * @brief Destructor.
		 */
		~Mat_t() = default;



		/**
		 * @brief Get number of rows.
		 * @return Number of rows.
		 */
		constexpr size_t rows() const
		{
			return M;
		}

		/**
		 * @brief Get number of columns.
		 * @return Number of columns.
		 */
		constexpr size_t columns() const
		{
			return N;
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<T, N> operator[](size_t index)
		{
			dk_assert(index < M);
			return Row<T, N>(data[index]);
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const T, N> operator[](size_t index) const
		{
			dk_assert(index < M);
			return Row<T, N>(data[index]);
		}

		/**
		 * @brief Convert the matrix into a string.
		 * @return The matrix as a string.
		 */
		std::string to_string() const
		{
			std::string str = "";

			for (size_t i = 0; i < M; ++i)
			{
				str += "[";
				for (size_t j = 0; j < N - 1; ++j)
					str += std::to_string(data[i][j]) + ", ";

				str += std::to_string(data[i][N - 1]) + "]\n";
			}
			str.resize(str.size() - 1);
			return str;
		}

		/**
		 * @brief Equivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are equal.
		 */
		bool operator==(const Mat_t<T, M, N>& other) const
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					if (data[i][j] != other.data[i][j])
						return false;

			return true;
		}

		/**
		 * @brief Unequivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are not equal.
		 */
		bool operator!=(const Mat_t<T, M, N>& other) const
		{
			return !(*this == other);
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, M, N>& operator=(const Mat_t<T2, M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = static_cast<T>(other.data[i][j]);

			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		template<typename T2>
		Mat_t<T, M, N> operator+(const Mat_t<T2, M, N>& other) const
		{
			Mat_t<T, M, N> mat = *this;

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] += static_cast<T>(other.data[i][j]);

			return mat;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		template<typename T2>
		Mat_t<T, M, N> operator-(const Mat_t<T2, M, N>& other) const
		{
			Mat_t<T, M, N> mat = *this;

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] -= static_cast<T>(other.data[i][j]);

			return mat;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of the other matrix.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<typename T2, size_t P>
		Mat_t<T, M, N> operator*(const Mat_t<T2, N, P>& other) const
		{
			Mat_t<T, M, P> mat(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < N; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<T, M> operator*(const Vec_t<T, M>& vec) const
		{
			Vec_t<T, M> new_vec(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					new_vec.data[i] += data[i][j] * vec.data[i];

			return new_vec;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @tparam Type of scalar.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator*(T2 scalar) const
		{
			Mat_t<T, 2, 2> mat = *this;
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] *= scalar;
			return mat;
		}

		/**
		 * @brief Scalar division operator.
		 * @tparam Type of scalar.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator/(T2 scalar) const
		{
			Mat_t<T, 2, 2> mat = *this;
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] /= scalar;
			return mat;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, M, N>& operator+=(const Mat_t<T2, M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] += static_cast<T>(other.data[i][j]);

			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, M, N>& operator-=(const Mat_t<T2, M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] -= static_cast<T>(other.data[i][j]);

			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of the other matrix.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2, size_t P>
		Mat_t<T, M, N>& operator*=(const Mat_t<T2, N, P>& other)
		{
			Mat_t<T, M, P> mat(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < N; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			*this = mat;
			return *this;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @tparam Type of scalar.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		template<typename T2>
		Mat_t<T, 2, 2>& operator*=(T2 scalar)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] *= scalar;
			return *this;
		}

		/**
		 * @brief Scalar division operator.
		 * @tparam Type of scalar.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		template<typename T2>
		Mat_t<T, 2, 2>& operator/=(T2 scalar)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] /= scalar;
			return *this;
		}

		/** Matrix data. */
		T data[M][N] = {};
	};



	/**
	 * @brief 2 by 2 matrix.
	 * @tparam Type used by the matrix.
	 */
	template<typename T>
	class alignas(16) Mat_t<T, 2, 2>
	{
	public:

		/**
		 * @brief Default constructor.
		 * @note Creates an identity matrix.
		 */
		Mat_t()
		{
			data[0][0] = 1;
			data[0][1] = 0;
			data[1][0] = 0;
			data[1][1] = 1;
		}

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		Mat_t(T val)
		{
			data[0][0] = val;
			data[1][0] = val;
			data[0][1] = val;
			data[1][1] = val;
		}
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(T a, T b, T c, T d)
		{
			data[0][0] = a;
			data[0][1] = b;
			data[1][0] = c;
			data[1][1] = d;
		}

		/**
		 * @brief Copy constructor.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 */
		template<typename T2>
		Mat_t(const Mat_t<T2, 2, 2>& other)
		{
			data[0][0] = static_cast<T>(other.data[0][0]);
			data[0][1] = static_cast<T>(other.data[1][0]);
			data[1][0] = static_cast<T>(other.data[0][1]);
			data[1][1] = static_cast<T>(other.data[1][1]);
		}

		/**
		 * @brief Destructor.
		 */
		~Mat_t() = default;



		/**
		 * @brief Get number of rows.
		 * @return Number of rows.
		 */
		constexpr size_t rows() const
		{
			return 2;
		}

		/**
		 * @brief Get number of columns.
		 * @return Number of columns.
		 */
		constexpr size_t columns() const
		{
			return 2;
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<T, 2> operator[](size_t index)
		{
			dk_assert(index < 2);
			return Row<T, 2>(data[index]);
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const T, 2> operator[](size_t index) const
		{
			dk_assert(index < 2);
			return Row<T, 2>(data[index]);
		}

		/**
		 * @brief Convert the matrix into a string.
		 * @return The matrix as a string.
		 */
		std::string to_string() const
		{
			std::string str = "";

			for (size_t i = 0; i < 2; ++i)
			{
				str += "[";
				for (size_t j = 0; j < 2 - 1; ++j)
					str += std::to_string(data[i][j]) + ", ";

				str += std::to_string(data[i][2 - 1]) + "]\n";
			}
			str.resize(str.size() - 1);
			return str;
		}

		/**
		 * @brief Equivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are equal.
		 */
		bool operator==(const Mat_t<T, 2, 2>& other) const
		{
			return	data[0][0] == other.data[0][0] && 
					data[1][0] == other.data[1][0] && 
					data[0][1] == other.data[0][1] && 
					data[1][1] == other.data[1][1];
		}

		/**
		 * @brief Unequivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are not equal.
		 */
		bool operator!=(const Mat_t<T, 2, 2>& other) const
		{
			return !(*this == other);
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, 2, 2>& operator=(const Mat_t<T2, 2, 2>& other)
		{
			data[0][0] = static_cast<T>(other.data[0][0]);
			data[1][0] = static_cast<T>(other.data[1][0]);
			data[0][1] = static_cast<T>(other.data[0][1]);
			data[1][1] = static_cast<T>(other.data[1][1]);
			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator+(const Mat_t<T2, 2, 2>& other) const
		{
			Mat_t<T, 2, 2> mat = *this;
			mat.data[0][0] += static_cast<T>(other.data[0][0]);
			mat.data[1][0] += static_cast<T>(other.data[1][0]);
			mat.data[0][1] += static_cast<T>(other.data[0][1]);
			mat.data[1][1] += static_cast<T>(other.data[1][1]);
			return mat;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator-(const Mat_t<T2, 2, 2>& other) const
		{
			Mat_t<T, 2, 2> mat = *this;
			mat.data[0][0] -= static_cast<T>(other.data[0][0]);
			mat.data[1][0] -= static_cast<T>(other.data[1][0]);
			mat.data[0][1] -= static_cast<T>(other.data[0][1]);
			mat.data[1][1] -= static_cast<T>(other.data[1][1]);
			return mat;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of the other matrix.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<typename T2, size_t P>
		Mat_t<T, 2, 2> operator*(const Mat_t<T2, 2, P>& other) const
		{
			Mat_t<T, 2, P> mat(0);

			for (size_t i = 0; i < 2; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 2; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<T, 2> operator*(const Vec_t<T, 2>& vec) const
		{
			Vec_t<T, 2> new_vec(0);
			new_vec.data[0] = (data[0][0] * vec.data[0]) + (data[0][1] * vec.data[1]);
			new_vec.data[1] = (data[1][0] * vec.data[0]) + (data[1][1] * vec.data[1]);
			return new_vec;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @tparam Type of scalar.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator*(T2 scalar) const
		{
			Mat_t<T, 2, 2> mat = *this;
			mat.data[0][0] *= static_cast<T>(scalar);
			mat.data[1][0] *= static_cast<T>(scalar);
			mat.data[0][1] *= static_cast<T>(scalar);
			mat.data[1][1] *= static_cast<T>(scalar);
			return mat;
		}

		/**
		 * @brief Scalar division operator.
		 * @tparam Type of scalar.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator/(T2 scalar) const
		{
			Mat_t<T, 2, 2> mat = *this;
			mat.data[0][0] /= static_cast<T>(scalar);
			mat.data[1][0] /= static_cast<T>(scalar);
			mat.data[0][1] /= static_cast<T>(scalar);
			mat.data[1][1] /= static_cast<T>(scalar);
			return mat;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, 2, 2>& operator+=(const Mat_t<T2, 2, 2>& other)
		{
			data[0][0] += static_cast<T>(other.data[0][0]);
			data[1][0] += static_cast<T>(other.data[1][0]);
			data[0][1] += static_cast<T>(other.data[0][1]);
			data[1][1] += static_cast<T>(other.data[1][1]);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, 2, 2>& operator-=(const Mat_t<T2, 2, 2>& other)
		{
			data[0][0] -= static_cast<T>(other.data[0][0]);
			data[1][0] -= static_cast<T>(other.data[1][0]);
			data[0][1] -= static_cast<T>(other.data[0][1]);
			data[1][1] -= static_cast<T>(other.data[1][1]);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of the other matrix.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2, size_t P>
		Mat_t<T, 2, 2>& operator*=(const Mat_t<T2, 2, P>& other)
		{
			Mat_t<T, 2, P> mat(0);

			for (size_t i = 0; i < 2; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 2; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			*this = mat;
			return *this;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @tparam Type of scalar.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator*=(T2 scalar)
		{
			data[0][0] *= static_cast<T>(scalar);
			data[1][0] *= static_cast<T>(scalar);
			data[0][1] *= static_cast<T>(scalar);
			data[1][1] *= static_cast<T>(scalar);
			return *this;
		}

		/**
		 * @brief Scalar division operator.
		 * @tparam Type of scalar.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		template<typename T2>
		Mat_t<T, 2, 2> operator/=(T2 scalar)
		{
			data[0][0] /= static_cast<T>(scalar);
			data[1][0] /= static_cast<T>(scalar);
			data[0][1] /= static_cast<T>(scalar);
			data[1][1] /= static_cast<T>(scalar);
			return *this;
		}

		/**
		 * @brief Determinant of the matrix.
		 * @return The determinant.
		 */
		T determinant() const
		{
			return (data[0][0] * data[1][1]) - (data[1][0] * data[0][1]);
		}

		/**
		 * @brief Get the inverse of the matrix.
		 * @return Inverse of the matrix.
		 */
		Mat_t<T, 2, 2> inverse() const
		{
			Mat_t<T, 2, 2> mat = 
			{
				 data[1][1], -data[0][1],
				-data[1][0],  data[0][0]
			};

			T det = determinant();
			dk_assert(det != 0);
			mat /= det;

			return mat;
		}

		/** Matrix data. */
		T data[2][2] = {};
	};

#if DUCK_USE_SIMD

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator+(const Mat_t<float, 2, 2>& other) const
	{
		Mat_t<float, 2, 2> mat = *this;
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_add_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator-(const Mat_t<float, 2, 2>& other) const
	{
		Mat_t<float, 2, 2> mat = *this;
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_sub_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator*(const Mat_t<float, 2, 2>& other) const
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][0], data[0][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_mul_ps(m1, m2);

		Mat_t<float, 2, 2> mat(0);
		mat.data[0][0] = m1.m128_f32[0] + m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2] + m1.m128_f32[3];

		m1 = { data[1][0], data[1][1], data[1][0], data[1][1] };
		m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_mul_ps(m1, m2);

		mat.data[1][0] = m1.m128_f32[0] + m1.m128_f32[1];
		mat.data[1][1] = m1.m128_f32[2] + m1.m128_f32[3];

		return mat;
	}

	template<>
	Vec_t<float, 2> Mat_t<float, 2, 2>::operator*(const Vec_t<float, 2>& vec) const
	{
		__m128 m1 = { data[0][0], data[0][1], data[1][0], data[1][1] };
		__m128 m2 = { vec.data[0], vec.data[1], vec.data[0], vec.data[1] };
		m1 = _mm_mul_ps(m1, m2);
		Vec_t<float, 2> new_vec(0);
		new_vec.data[0] = m1.m128_f32[0] + m1.m128_f32[1];
		new_vec.data[1] = m1.m128_f32[2] + m1.m128_f32[3];
		return new_vec;
	}

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator*(float scalar) const
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		Mat_t<float, 2, 2> mat = *this;
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator/(float scalar) const
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		Mat_t<float, 2, 2> mat = *this;
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	template<>
	template<>
	Mat_t<float, 2, 2>& Mat_t<float, 2, 2>::operator+=(const Mat_t<float, 2, 2>& other)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_add_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	template<>
	template<>
	Mat_t<float, 2, 2>& Mat_t<float, 2, 2>::operator-=(const Mat_t<float, 2, 2>& other)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_sub_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	template<>
	template<>
	Mat_t<float, 2, 2>& Mat_t<float, 2, 2>::operator*=(const Mat_t<float, 2, 2>& other)
	{
		*this = *this * other;
		return *this;
	}

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator*=(float scalar)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	template<>
	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::operator/=(float scalar)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	template<>
	Mat_t<float, 2, 2> Mat_t<float, 2, 2>::inverse() const
	{
		Mat_t<float, 2, 2> mat ={ data[1][1], -data[0][1], -data[1][0],  data[0][0] };
		float det = determinant();
		dk_assert(det != 0);
		mat /= det;
		return mat;
	}

#endif



	/**
	 * @brief 3 by 3 matrix.
	 * @tparam Type used by the matrix.
	 */
	template<typename T>
	class alignas(16) Mat_t<T, 3, 3>
	{
	public:

		/**
		 * @brief Default constructor.
		 * @note Creates an identity matrix.
		 */
		Mat_t()
		{
			data[0][0] = 1;
			data[0][1] = 0;
			data[0][2] = 0;

			data[1][0] = 0;
			data[1][1] = 1;
			data[1][2] = 0;

			data[2][0] = 0;
			data[2][1] = 0;
			data[2][2] = 1;
		}

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		Mat_t(T val)
		{
			data[0][0] = val;
			data[0][1] = val;
			data[0][2] = val;

			data[1][0] = val;
			data[1][1] = val;
			data[1][2] = val;

			data[2][0] = val;
			data[2][1] = val;
			data[2][2] = val;
		}
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(T a, T b, T c, T d, T e, T f, T g, T h, T i)
		{
			data[0][0] = a;
			data[0][1] = b;
			data[0][2] = c;

			data[1][0] = d;
			data[1][1] = e;
			data[1][2] = f;

			data[2][0] = g;
			data[2][1] = h;
			data[2][2] = i;
		}

		/**
		 * @brief Copy constructor.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 */
		template<typename T2>
		Mat_t(const Mat_t<T2, 3, 3>& other)
		{
			data[0][0] = static_cast<T>(other.data[0][0]);
			data[0][1] = static_cast<T>(other.data[0][1]);
			data[0][2] = static_cast<T>(other.data[0][2]);

			data[1][0] = static_cast<T>(other.data[1][0]);
			data[1][1] = static_cast<T>(other.data[1][1]);
			data[1][2] = static_cast<T>(other.data[1][2]);

			data[2][0] = static_cast<T>(other.data[2][0]);
			data[2][1] = static_cast<T>(other.data[2][1]);
			data[2][2] = static_cast<T>(other.data[2][2]);
		}

		/**
		 * @brief Destructor.
		 */
		~Mat_t() = default;



		/**
		 * @brief Get number of rows.
		 * @return Number of rows.
		 */
		constexpr size_t rows() const
		{
			return 3;
		}

		/**
		 * @brief Get number of columns.
		 * @return Number of columns.
		 */
		constexpr size_t columns() const
		{
			return 3;
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<T, 3> operator[](size_t index)
		{
			dk_assert(index < 3);
			return Row<T, 3>(data[index]);
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const T, 3> operator[](size_t index) const
		{
			dk_assert(index < 3);
			return Row<T, 3>(data[index]);
		}

		/**
		 * @brief Convert the matrix into a string.
		 * @return The matrix as a string.
		 */
		std::string to_string() const
		{
			std::string str = "";

			for (size_t i = 0; i < 3; ++i)
			{
				str += "[";
				for (size_t j = 0; j < 3 - 1; ++j)
					str += std::to_string(data[i][j]) + ", ";

				str += std::to_string(data[i][3 - 1]) + "]\n";
			}
			str.resize(str.size() - 1);
			return str;
		}

		/**
		 * @brief Equivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are equal.
		 */
		bool operator==(const Mat_t<T, 3, 3>& other) const
		{
			return	data[0][0] == other.data[0][0] && 
					data[0][1] == other.data[0][1] && 
					data[0][2] == other.data[0][2] &&
					data[1][0] == other.data[1][0] &&
					data[1][1] == other.data[1][1] &&
					data[1][2] == other.data[1][2] && 
					data[2][0] == other.data[2][0] &&
					data[2][1] == other.data[2][1] &&
					data[2][2] == other.data[2][2] &&;
		}

		/**
		 * @brief Unequivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are not equal.
		 */
		bool operator!=(const Mat_t<T, 3, 3>& other) const
		{
			return !(*this == other);
		}

		/**
		 * @brief Assignment operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, 3, 3>& operator=(const Mat_t<T2, 3, 3>& other)
		{
			data[0][0] = static_cast<T>(other.data[0][0]);
			data[0][1] = static_cast<T>(other.data[0][1]);
			data[0][2] = static_cast<T>(other.data[0][2]);
			data[1][0] = static_cast<T>(other.data[1][0]);
			data[1][1] = static_cast<T>(other.data[1][1]);
			data[1][2] = static_cast<T>(other.data[1][2]);
			data[2][0] = static_cast<T>(other.data[2][0]);
			data[2][1] = static_cast<T>(other.data[2][1]);
			data[2][2] = static_cast<T>(other.data[2][2]);
			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		template<typename T2>
		Mat_t<T, 3, 3> operator+(const Mat_t<T2, 3, 3>& other) const
		{
			Mat_t<T, 3, 3> mat = *this;
			mat.data[0][0] += static_cast<T>(other.data[0][0]);
			mat.data[0][1] += static_cast<T>(other.data[0][1]);
			mat.data[0][2] += static_cast<T>(other.data[0][2]);
			mat.data[1][0] += static_cast<T>(other.data[1][0]);
			mat.data[1][1] += static_cast<T>(other.data[1][1]);
			mat.data[1][2] += static_cast<T>(other.data[1][2]);
			mat.data[2][0] += static_cast<T>(other.data[2][0]);
			mat.data[2][1] += static_cast<T>(other.data[2][1]);
			mat.data[2][2] += static_cast<T>(other.data[2][2]);
			return mat;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		template<typename T2>
		Mat_t<T, 3, 3> operator-(const Mat_t<T2, 3, 3>& other) const
		{
			Mat_t<T, 3, 3> mat = *this;
			mat.data[0][0] -= static_cast<T>(other.data[0][0]);
			mat.data[0][1] -= static_cast<T>(other.data[0][1]);
			mat.data[0][2] -= static_cast<T>(other.data[0][2]);
			mat.data[1][0] -= static_cast<T>(other.data[1][0]);
			mat.data[1][1] -= static_cast<T>(other.data[1][1]);
			mat.data[1][2] -= static_cast<T>(other.data[1][2]);
			mat.data[2][0] -= static_cast<T>(other.data[2][0]);
			mat.data[2][1] -= static_cast<T>(other.data[2][1]);
			mat.data[2][2] -= static_cast<T>(other.data[2][2]);
			return mat;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of the other matrix.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<typename T2, size_t P>
		Mat_t<T, 3, 3> operator*(const Mat_t<T2, 3, P>& other) const
		{
			Mat_t<T, 3, P> mat(0);

			for (size_t i = 0; i < 3; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 3; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<T, 3> operator*(const Vec_t<T, 3>& vec) const
		{
			Vec_t<T, 3> new_vec(0);
			new_vec.data[0] = (data[0][0] * vec.data[0]) + (data[0][1] * vec.data[1]) + (data[0][2] * vec.data[2]);
			new_vec.data[1] = (data[1][0] * vec.data[0]) + (data[1][1] * vec.data[1]) + (data[1][2] * vec.data[2]);
			new_vec.data[2] = (data[2][0] * vec.data[0]) + (data[2][1] * vec.data[1]) + (data[2][2] * vec.data[2]);
			return new_vec;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @tparam Type of scalar.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		template<typename T2>
		Mat_t<T, 3, 3> operator*(T2 scalar) const
		{
			Mat_t<T, 3, 3> mat = *this;
			mat.data[0][0] *= static_cast<T>(scalar);
			mat.data[0][1] *= static_cast<T>(scalar);
			mat.data[0][2] *= static_cast<T>(scalar);
			mat.data[1][0] *= static_cast<T>(scalar);
			mat.data[1][1] *= static_cast<T>(scalar);
			mat.data[1][2] *= static_cast<T>(scalar);
			mat.data[2][0] *= static_cast<T>(scalar);
			mat.data[2][1] *= static_cast<T>(scalar);
			mat.data[2][2] *= static_cast<T>(scalar);
			return mat;
		}

		/**
		 * @brief Scalar division operator.
		 * @tparam Type of scalar.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		template<typename T2>
		Mat_t<T, 3, 3> operator/(T2 scalar) const
		{
			Mat_t<T, 3, 3> mat = *this;
			mat.data[0][0] /= static_cast<T>(scalar);
			mat.data[0][1] /= static_cast<T>(scalar);
			mat.data[0][2] /= static_cast<T>(scalar);
			mat.data[1][0] /= static_cast<T>(scalar);
			mat.data[1][1] /= static_cast<T>(scalar);
			mat.data[1][2] /= static_cast<T>(scalar);
			mat.data[2][0] /= static_cast<T>(scalar);
			mat.data[2][1] /= static_cast<T>(scalar);
			mat.data[2][2] /= static_cast<T>(scalar);
			return mat;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, 3, 3>& operator+=(const Mat_t<T2, 3, 3>& other)
		{
			data[0][0] += static_cast<T>(other.data[0][0]);
			data[0][1] += static_cast<T>(other.data[0][1]);
			data[0][2] += static_cast<T>(other.data[0][2]);
			data[1][0] += static_cast<T>(other.data[1][0]);
			data[1][1] += static_cast<T>(other.data[1][1]);
			data[1][2] += static_cast<T>(other.data[1][2]);
			data[2][0] += static_cast<T>(other.data[2][0]);
			data[2][1] += static_cast<T>(other.data[2][1]);
			data[2][2] += static_cast<T>(other.data[2][2]);
			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		Mat_t<T, 3, 3>& operator-=(const Mat_t<T2, 3, 3>& other)
		{
			data[0][0] -= static_cast<T>(other.data[0][0]);
			data[0][1] -= static_cast<T>(other.data[0][1]);
			data[0][2] -= static_cast<T>(other.data[0][2]);
			data[1][0] -= static_cast<T>(other.data[1][0]);
			data[1][1] -= static_cast<T>(other.data[1][1]);
			data[1][2] -= static_cast<T>(other.data[1][2]);
			data[2][0] -= static_cast<T>(other.data[2][0]);
			data[2][1] -= static_cast<T>(other.data[2][1]);
			data[2][2] -= static_cast<T>(other.data[2][2]);
			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Type of the other matrix.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2, size_t P>
		Mat_t<T, 3, 3>& operator*=(const Mat_t<T2, 3, P>& other)
		{
			Mat_t<T, 3, P> mat(0);

			for (size_t i = 0; i < 3; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 3; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			*this = mat;
			return *this;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @tparam Type of scalar.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		template<typename T2>
		Mat_t<T, 3, 3> operator*=(T2 scalar)
		{
			data[0][0] *= static_cast<T>(scalar);
			data[0][1] *= static_cast<T>(scalar);
			data[0][2] *= static_cast<T>(scalar);
			data[1][0] *= static_cast<T>(scalar);
			data[1][1] *= static_cast<T>(scalar);
			data[1][2] *= static_cast<T>(scalar);
			data[2][0] *= static_cast<T>(scalar);
			data[2][1] *= static_cast<T>(scalar);
			data[2][2] *= static_cast<T>(scalar);
			return *this;
		}

		/**
		 * @brief Scalar division operator.
		 * @tparam Type of scalar.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		template<typename T2>
		Mat_t<T, 3, 3> operator/=(T2 scalar)
		{
			data[0][0] /= static_cast<T>(scalar);
			data[0][1] /= static_cast<T>(scalar);
			data[0][2] /= static_cast<T>(scalar);
			data[1][0] /= static_cast<T>(scalar);
			data[1][1] /= static_cast<T>(scalar);
			data[1][2] /= static_cast<T>(scalar);
			data[2][0] /= static_cast<T>(scalar);
			data[2][1] /= static_cast<T>(scalar);
			data[2][2] /= static_cast<T>(scalar);
			return *this;
		}

		/**
		 * @brief Determinant of the matrix.
		 * @return The determinant.
		 */
		T determinant() const
		{
			return	(data[0][0] * data[1][1] * data[2][2]) + (data[0][1] * data[1][2] * data[2][0]) + 
					(data[0][2] * data[1][0] * data[2][1]) - (data[0][2] * data[1][1] * data[2][0]) - 
					(data[0][1] * data[1][0] * data[2][2]) - (data[0][0] * data[1][2] * data[2][1]);
		}

		/**
		 * @brief Get the inverse of the matrix.
		 * @return Inverse of the matrix.
		 */
		Mat_t<T, 3, 3> inverse() const
		{
			Mat_t<T, 3, 3> mat = 
			{
				(data[1][1]*data[2][2]) - (data[1][2]*data[2][1]), (data[0][2]*data[2][1]) - (data[0][1]*data[2][2]), (data[0][1]*data[1][2]) - (data[0][2]*data[1][1]),
				(data[1][2]*data[2][0]) - (data[1][0]*data[2][2]), (data[0][0]*data[2][2]) - (data[0][2]*data[2][0]), (data[0][2]*data[1][0]) - (data[0][0]*data[1][2]),
				(data[1][0]*data[2][1]) - (data[1][1]*data[2][0]), (data[0][1]*data[2][0]) - (data[0][0]*data[2][1]), (data[0][0]*data[1][1]) - (data[0][1]*data[1][0])
			};

			T det = determinant();
			dk_assert(det != 0);
			mat /= det;

			return mat;
		}

		/** Matrix data. */
		T data[3][3] = {};
	};



	/** Standard 4D matrix. */
	using Mat4 = Mat_t<RealNumber, 4, 4>;

	/** Floating point 4D matrix. */
	using Mat4f = Mat_t<float, 4, 4>;

	/** Double precision floating point 4D matrix. */
	using Mat4d = Mat_t<double, 4, 4>;

	/** Signed integer 4D matrix. */
	using Mat4i = Mat_t<int32_t, 4, 4>;



	/** Standard 3D matrix. */
	using Mat3 = Mat_t<RealNumber, 3, 3>;

	/** Floating point 3D matrix. */
	using Mat3f = Mat_t<float, 3, 3>;

	/** Double precision floating point 2D matrix. */
	using Mat3d = Mat_t<double, 3, 3>;

	/** Signed integer 2D matrix. */
	using Mat3i = Mat_t<int32_t, 3, 3>;



	/** Standard 2D matrix. */
	using Mat2 = Mat_t<RealNumber, 2, 2>;

	/** Floating point 2D matrix. */
	using Mat2f = Mat_t<float, 2, 2>;

	/** Double precision floating point 2D matrix. */
	using Mat2d = Mat_t<double, 2, 2>;

	/** Signed integer 2D matrix. */
	using Mat2i = Mat_t<int32_t, 2, 2>;
}