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
	 * @brief M by N matrix.
	 * @tparam Type used by the matrix.
	 * @tparam M dimension.
	 * @tparam N dimension.
	 */
	template<typename T, size_t M, size_t N>
	class mat_t
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