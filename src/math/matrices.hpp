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
	 * @tparam Type of data sent to the row.
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
	 * @tparam M dimension.
	 * @tparam N dimension.
	 */
	template<size_t M, size_t N>
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
		Mat_t(float val)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = val;
		}
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(std::initializer_list<float> values)
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
		 * @param Other matrix.
		 */
		Mat_t(const Mat_t<M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = other.data[i][j];
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
		Row<float, N> operator[](size_t index)
		{
			dk_assert(index < M);
			return Row<float, N>(data[index]);
		}

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const float, N> operator[](size_t index) const
		{
			dk_assert(index < M);
			return Row<float, N>(data[index]);
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
		bool operator==(const Mat_t<M, N>& other) const
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
		bool operator!=(const Mat_t<M, N>& other) const
		{
			return !(*this == other);
		}

		/**
		 * @brief Assignment operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<M, N>& operator=(const Mat_t<M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = other.data[i][j];

			return *this;
		}

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		Mat_t<M, N> operator+(const Mat_t<M, N>& other) const
		{
			Mat_t<M, N> mat = *this;

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] += other.data[i][j];

			return mat;
		}

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		Mat_t<M, N> operator-(const Mat_t<M, N>& other) const
		{
			Mat_t<M, N> mat = *this;

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] -= other.data[i][j];

			return mat;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<size_t P>
		Mat_t<M, N> operator*(const Mat_t<N, P>& other) const
		{
			Mat_t<M, P> mat(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < N; ++k)
						mat.data[i][j] += data[i][k] * other.data[k][j];

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<M> operator*(const Vec_t<M>& vec) const
		{
			Vec_t<M> new_vec(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					new_vec.data[i] += data[i][j] * vec.data[i];

			return new_vec;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		Mat_t<M, N> operator*(float scalar) const
		{
			Mat_t<M, N> mat = *this;
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] *= scalar;
			return mat;
		}

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		Mat_t<M, N> operator/(float scalar) const
		{
			Mat_t<M, N> mat = *this;
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					mat.data[i][j] /= scalar;
			return mat;
		}

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<M, N>& operator+=(const Mat_t<M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] += other.data[i][j];

			return *this;
		}

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<M, N>& operator-=(const Mat_t<M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] -= other.data[i][j];

			return *this;
		}

		/**
		 * @brief Multiplication operator.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<size_t P>
		Mat_t<M, N>& operator*=(const Mat_t<N, P>& other)
		{
			Mat_t<M, P> mat(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < N; ++k)
						mat.data[i][j] += data[i][k] * other.data[k][j];

			*this = mat;
			return *this;
		}

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		Mat_t<M, N>& operator*=(float scalar)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] *= scalar;
			return *this;
		}

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		Mat_t<M, N>& operator/=(float scalar)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] /= scalar;
			return *this;
		}

		/** Matrix data. */
		float data[M][N] = {};
	};



	/**
	 * @brief 2 by 2 matrix.
	 * @tparam Type used by the matrix.
	 */
	template<>
	class alignas(16) Mat_t<2, 2>
	{
	public:

		/**
		 * @brief Default constructor.
		 * @note Creates an identity matrix.
		 */
		Mat_t();

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		Mat_t(float val);
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(float a, float b, float c, float d);

		/**
		 * @brief Copy constructor.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 */
		Mat_t(const Mat_t<2, 2>& other);

		/**
		 * @brief Destructor.
		 */
		~Mat_t() = default;

		/**
		 * @brief Get number of rows.
		 * @return Number of rows.
		 */
		constexpr size_t rows() const;

		/**
		 * @brief Get number of columns.
		 * @return Number of columns.
		 */
		constexpr size_t columns() const;

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<float, 2> operator[](size_t index);

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const float, 2> operator[](size_t index) const;

		/**
		 * @brief Convert the matrix into a string.
		 * @return The matrix as a string.
		 */
		std::string to_string() const;

		/**
		 * @brief Equivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are equal.
		 */
		bool operator==(const Mat_t<2, 2>& other) const;

		/**
		 * @brief Unequivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are not equal.
		 */
		bool operator!=(const Mat_t<2, 2>& other) const;

		/**
		 * @brief Assignment operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<2, 2>& operator=(const Mat_t<2, 2>& other);

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		Mat_t<2, 2> operator+(const Mat_t<2, 2>& other) const;

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		Mat_t<2, 2> operator-(const Mat_t<2, 2>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<size_t P>
		Mat_t<2, P> operator*(const Mat_t<2, P>& other) const
		{
			Mat_t<2, P> mat(0);

			for (size_t i = 0; i < 2; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 2; ++k)
						mat.data[i][j] += data[i][k] * other.data[k][j];

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<2> operator*(const Vec_t<2>& vec) const;

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		Mat_t<2, 2> operator*(float scalar) const;

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		Mat_t<2, 2> operator/(float scalar) const;

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<2, 2>& operator+=(const Mat_t<2, 2>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<2, 2>& operator-=(const Mat_t<2, 2>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<2, 2>& operator*=(const Mat_t<2, 2>& other);

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		Mat_t<2, 2> operator*=(float scalar);

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		Mat_t<2, 2> operator/=(float scalar);

		/**
		 * @brief Determinant of the matrix.
		 * @return The determinant.
		 */
		float determinant() const;

		/**
		 * @brief Get the inverse of the matrix.
		 * @return Inverse of the matrix.
		 */
		Mat_t<2, 2> inverse() const;

		/** Matrix data. */
		float data[2][2] = {};
	};



	/**
	 * @brief 3 by 3 matrix.
	 * @tparam Type used by the matrix.
	 */
	template<>
	class alignas(16) Mat_t<3, 3>
	{
	public:

		/**
		 * @brief Default constructor.
		 * @note Creates an identity matrix.
		 */
		Mat_t();

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		Mat_t(float val);
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(float a, float b, float c, float d, float e, float f, float g, float h, float i);

		/**
		 * @brief Copy constructor.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 */
		Mat_t(const Mat_t<3, 3>& other);

		/**
		 * @brief Destructor.
		 */
		~Mat_t() = default;

		/**
		 * @brief Get number of rows.
		 * @return Number of rows.
		 */
		constexpr size_t rows() const;

		/**
		 * @brief Get number of columns.
		 * @return Number of columns.
		 */
		constexpr size_t columns() const;

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<float, 3> operator[](size_t index);

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const float, 3> operator[](size_t index) const;

		/**
		 * @brief Convert the matrix into a string.
		 * @return The matrix as a string.
		 */
		std::string to_string() const;

		/**
		 * @brief Equivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are equal.
		 */
		bool operator==(const Mat_t<3, 3>& other) const;

		/**
		 * @brief Unequivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are not equal.
		 */
		bool operator!=(const Mat_t<3, 3>& other) const;

		/**
		 * @brief Assignment operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<3, 3>& operator=(const Mat_t<3, 3>& other);

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		Mat_t<3, 3> operator+(const Mat_t<3, 3>& other) const;

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		Mat_t<3, 3> operator-(const Mat_t<3, 3>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<size_t P>
		Mat_t<3, 3> operator*(const Mat_t<3, P>& other) const
		{
			Mat_t<3, P> mat(0);

			for (size_t i = 0; i < 3; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 3; ++k)
						mat.data[i][j] += data[i][k] * other.data[k][j];

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<3> operator*(const Vec_t<3>& vec) const;

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		Mat_t<3, 3> operator*(float scalar) const;

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		Mat_t<3, 3> operator/(float scalar) const;

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<3, 3>& operator+=(const Mat_t<3, 3>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<3, 3>& operator-=(const Mat_t<3, 3>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<3, 3>& operator*=(const Mat_t<3, 3>& other);

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		Mat_t<3, 3> operator*=(float scalar);

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		Mat_t<3, 3> operator/=(float scalar);

		/**
		 * @brief Determinant of the matrix.
		 * @return The determinant.
		 */
		float determinant() const;

		/**
		 * @brief Get the inverse of the matrix.
		 * @return Inverse of the matrix.
		 */
		Mat_t<3, 3> inverse() const;

		/** Matrix data. */
		float data[3][3] = {};
	};



	/**
	 * @brief 4 by 4 matrix.
	 */
	template<>
	class alignas(16) Mat_t<4, 4>
	{
	public:

		/**
		 * @brief Default constructor.
		 * @note Creates an identity matrix.
		 */
		Mat_t();

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		Mat_t(float val);
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		Mat_t(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p);

		/**
		 * @brief Copy constructor.
		 * @param Other matrix.
		 */
		Mat_t(const Mat_t<4, 4>& other);

		/**
		 * @brief Destructor.
		 */
		~Mat_t() = default;

		/**
		 * @brief Get number of rows.
		 * @return Number of rows.
		 */
		constexpr size_t rows() const;

		/**
		 * @brief Get number of columns.
		 * @return Number of columns.
		 */
		constexpr size_t columns() const;

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<float, 4> operator[](size_t index);

		/**
		 * @brief Access operator.
		 * @param Row index.
		 * @return Row at the given index. 
		 */
		Row<const float, 4> operator[](size_t index) const;

		/**
		 * @brief Convert the matrix into a string.
		 * @return The matrix as a string.
		 */
		std::string to_string() const;

		/**
		 * @brief Equivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are equal.
		 */
		bool operator==(const Mat_t<4, 4>& other) const;

		/**
		 * @brief Unequivilence operator.
		 * @param Other matrix.
		 * @return If the two matrices are not equal.
		 */
		bool operator!=(const Mat_t<4, 4>& other) const;

		/**
		 * @brief Assignment operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<4, 4>& operator=(const Mat_t<4, 4>& other);

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return Sum of the two matrices.
		 */
		Mat_t<4, 4> operator+(const Mat_t<4, 4>& other) const;

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return Difference between the two matrices.
		 */
		Mat_t<4, 4> operator-(const Mat_t<4, 4>& other) const;

		/**
		 * @brief Multiplication operator.
		 * @tparam Number of columns in the other matrix
		 * @param Other matrix.
		 * @return Product of the two matrices.
		 */
		template<size_t P>
		Mat_t<4, P> operator*(const Mat_t<4, P>& other) const
		{
			Mat_t<4, P> mat(0);

			for (size_t i = 0; i < 4; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < 4; ++k)
						mat.data[i][j] += data[i][k] * other.data[k][j];

			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		Vec_t<4> operator*(const Vec_t<4>& vec) const;

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return Matrix with multiplied values.
		 */
		Mat_t<4, 4> operator*(float scalar) const;

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return Matrix with quotient values.
		 */
		Mat_t<4, 4> operator/(float scalar) const;

		/**
		 * @brief Addition operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<4, 4>& operator+=(const Mat_t<4, 4>& other);

		/**
		 * @brief Subtraction operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<4, 4>& operator-=(const Mat_t<4, 4>& other);

		/**
		 * @brief Multiplication operator.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		Mat_t<4, 4>& operator*=(const Mat_t<4, 4>& other);

		/**
		 * @brief Scalar multiplication operator.
		 * @param Scalar to multiply.
		 * @return This matrix
		 */
		Mat_t<4, 4> operator*=(float scalar);

		/**
		 * @brief Scalar division operator.
		 * @param Scalar to divide.
		 * @return This matrix
		 */
		Mat_t<4, 4> operator/=(float scalar);

		/**
		 * @brief Determinant of the matrix.
		 * @return The determinant.
		 */
		float determinant() const;

		/**
		 * @brief Get the inverse of the matrix.
		 * @return Inverse of the matrix.
		 * @note Thanks to the ID Software for this code ;)
		 */
		Mat_t<4, 4> inverse() const;

		/** Matrix data. */
		float data[4][4] = {};
	};

#if DUCK_USE_SIMD

	template<>
	Mat_t<2, 2> Mat_t<2, 2>::operator*(const Mat_t<2, 2>& other) const;

	template<>
	Mat_t<3, 3> Mat_t<3, 3>::operator*(const Mat_t<3, 3>& other) const;

	template<>
	Mat_t<4, 4> Mat_t<4, 4>::operator*(const Mat_t<4, 4>& other) const;

#endif

	/** Standard 4D matrix. */
	using Mat4 = Mat_t<4, 4>;

	/** Standard 3D matrix. */
	using Mat3 = Mat_t<3, 3>;

	/** Standard 2D matrix. */
	using Mat2 = Mat_t<2, 2>;
}