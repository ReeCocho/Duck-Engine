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
	class alignas(16) mat_t
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		mat_t() = default;

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		mat_t(T val)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = val;
		}
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		mat_t(std::initializer_list<T> values)
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
		mat_t(const mat_t<T2, M, N>& other)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = static_cast<T>(other.data[i][j]);
		}

		/**
		 * @brief Destructor.
		 */
		~mat_t() = default;



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
		bool operator==(const mat_t<T, M, N>& other) const
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
		bool operator!=(const mat_t<T, M, N>& other) const
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
		mat_t<T, M, N>& operator=(const mat_t<T2, M, N>& other)
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
		mat_t<T, M, N> operator+(const mat_t<T2, M, N>& other) const
		{
			mat_t<T, M, N> mat = *this;

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
		mat_t<T, M, N> operator-(const mat_t<T2, M, N>& other) const
		{
			mat_t<T, M, N> mat = *this;

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
		mat_t<T, M, N> operator*(const mat_t<T2, N, P>& other) const
		{
			mat_t<T, M, P> mat(0);

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
		vec_t<T, M> operator*(const vec_t<T, M>& vec) const
		{
			vec_t<T, M> new_vec(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					new_vec.data[i] += data[i][j] * vec.data[i];

			return new_vec;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		mat_t<T, M, N>& operator+=(const mat_t<T2, M, N>& other)
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
		mat_t<T, M, N>& operator-=(const mat_t<T2, M, N>& other)
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
		mat_t<T, M, N>& operator*=(const mat_t<T2, N, P>& other)
		{
			mat_t<T, M, P> mat(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < N; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			*this = mat;
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
	class alignas(16) mat_t<T, 2, 2>
	{
	public:

		/**
		 * @brief Default constructor.
		 * @note Creates an identity matrix.
		 */
		mat_t()
		{
			data[0][0] = 1;
			data[1][0] = 0;
			data[0][1] = 1;
			data[1][1] = 0;
		}

		/**
		 * @brief Constructor.
		 * @param Value for every element.
		 */
		mat_t(T val)
		{
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					data[i][j] = val;
		}
		
		/**
		 * @brief Constructor.
		 * @param Matrix values.
		 */
		mat_t(std::initializer_list<T> values)
		{
			dk_assert(values.size() == 4);

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
		mat_t(const mat_t<T2, 2, 2>& other)
		{
			data[0][0] = static_cast<T>(other.data[0][0]);
			data[1][0] = static_cast<T>(other.data[1][0]);
			data[0][1] = static_cast<T>(other.data[0][1]);
			data[1][1] = static_cast<T>(other.data[1][1]);
		}

		/**
		 * @brief Destructor.
		 */
		~mat_t() = default;



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
		Row<T, N> operator[](size_t index)
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
		bool operator==(const mat_t<T, 2, 2>& other) const
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
		bool operator!=(const mat_t<T, M, N>& other) const
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
		mat_t<T, M, N>& operator=(const mat_t<T2, M, N>& other)
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
		mat_t<T, M, N> operator+(const mat_t<T2, M, N>& other) const
		{
			mat_t<T, M, N> mat = *this;
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
		mat_t<T, M, N> operator-(const mat_t<T2, M, N>& other) const
		{
			mat_t<T, M, N> mat = *this;
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
		mat_t<T, M, N> operator*(const mat_t<T2, N, P>& other) const
		{
			mat_t<T, M, P> mat(0);
			return mat;
		}

		/**
		 * @brief Vector multiplication operator.
		 * @param Vector to multiply.
		 * @return Vector with resulting multiplied values.
		 */
		vec_t<T, M> operator*(const vec_t<T, M>& vec) const
		{
			vec_t<T, M> new_vec(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					new_vec.data[i] += data[i][j] * vec.data[i];

			return new_vec;
		}

		/**
		 * @brief Addition operator.
		 * @tparam Type of the other matrix.
		 * @param Other matrix.
		 * @return This matrix.
		 */
		template<typename T2>
		mat_t<T, M, N>& operator+=(const mat_t<T2, M, N>& other)
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
		mat_t<T, M, N>& operator-=(const mat_t<T2, M, N>& other)
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
		mat_t<T, M, N>& operator*=(const mat_t<T2, N, P>& other)
		{
			mat_t<T, M, P> mat(0);

			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < P; ++j)
					for (size_t k = 0; k < N; ++k)
						mat.data[i][j] += data[i][k] * static_cast<T>(other.data[k][j]);

			*this = mat;
			return *this;
		}

		/** Matrix data. */
		T data[2][2] = {};
	};



	/** Standard 4D vector. */
	using mat4 = mat_t<RealNumber, 4, 4>;

	/** Floating point 4D vector. */
	using mat4f = mat_t<float, 4, 4>;

	/** Double precision floating point 4D vector. */
	using mat4d = mat_t<double, 4, 4>;

	/** Signed integer 4D vector. */
	using mat4i = mat_t<int32_t, 4, 4>;



	/** Standard 3D vector. */
	using mat3 = mat_t<RealNumber, 3, 3>;

	/** Floating point 3D vector. */
	using mat3f = mat_t<float, 3, 3>;

	/** Double precision floating point 2D vector. */
	using mat3d = mat_t<double, 3, 3>;

	/** Signed integer 2D vector. */
	using mat3i = mat_t<int32_t, 3, 3>;



	/** Standard 2D vector. */
	using mat2 = mat_t<RealNumber, 2, 2>;

	/** Floating point 2D vector. */
	using mat2f = mat_t<float, 2, 2>;

	/** Double precision floating point 2D vector. */
	using mat2d = mat_t<double, 2, 2>;

	/** Signed integer 2D vector. */
	using mat2i = mat_t<int32_t, 2, 2>;
}