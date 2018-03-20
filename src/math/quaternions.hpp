#pragma once

/**
 * @file quaternions.hpp
 * @brief Duck math quaternions.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vectors.hpp"
#include "matrices.hpp"

namespace dk
{
	/**
	 * @brief A 4D vector used for rotations. (Quaternion)
	 */
	class Quat
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Quat();

		/**
		 * @brief Constructor.
		 * @param X value.
		 * @param Y value.
		 * @param Z value.
		 * @param W value.
		 */
		Quat(float x, float y, float z, float w);

		/**
		 * @brief Copy constructor.
		 * @param Other quaternion.
		 */
		Quat(const Quat& other);

		/**
		 * @brief Constructor.
		 * @param Euler angles.
		 */
		Quat(const Vec_t<3>& euler);

		/**
		 * @brief Constructor.
		 * @param 4D vector.
		 */
		Quat(const Vec_t<4>& vec);

		/**
		 * @brief Default destructor.
		 */
		~Quat() = default;



		/**
		 * @brief Equivilence operator.
		 * @param Other quaternion.
		 * @return If the this quaternion is the same as the other.
		 */
		bool operator==(const Quat& other) const;

		/**
		 * @brief Equivilence operator.
		 * @param 4D vector.
		 * @return If the this quaternion is the same as the vector.
		 */
		bool operator==(const Vec_t<4>& vec) const;

		/**
		 * @brief Unequivilence operator.
		 * @param Other quaternion.
		 * @return If the this quaternion is not the same as the other.
		 */
		bool operator!=(const Quat& other) const;

		/**
		 * @brief Unequivilence operator.
		 * @param 4D vector.
		 * @return If the this quaternion is not the same as the vector.
		 */
		bool operator!=(const Vec_t<4>& vec) const;

		/**
		 * @brief Assignment operator.
		 * @param Other quaternion.
		 * @return This quaternion.
		 */
		Quat& operator=(const Quat& other);

		/**
		 * @brief Assignment operator.
		 * @param 4D vector.
		 * @return This quaternion.
		 */
		Quat& operator=(const Vec_t<4>& vec);

		/**
		 * @brief Addition operator.
		 * @param Other quaternion.
		 * @return New quaternion with summed values.
		 */
		Quat operator+(const Quat& other) const;

		/**
		 * @brief Addition operator.
		 * @param 4D vector.
		 * @return New quaternion with summed values.
		 */
		Quat operator+(const Vec_t<4>& vec) const;

		/**
		 * @brief Subtraction operator.
		 * @param Other quaternion.
		 * @return New quaternion with difference values.
		 */
		Quat operator-(const Quat& other) const;

		/**
		 * @brief Subtraction operator.
		 * @param 4D vector.
		 * @return New quaternion with difference values.
		 */
		Quat operator-(const Vec_t<4>& vec) const;

		/**
		 * @brief Multiplication operator.
		 * @param Other quaternion.
		 * @return New quaternion with product values.
		 */
		Quat operator*(const Quat& other) const;

		/**
		 * @brief Multiplication operator.
		 * @param 4D vector.
		 * @return New quaternion with product values.
		 */
		Quat operator*(const Vec_t<4>& vec) const;

		/**
		 * @brief Multiplication operator.
		 * @param Scalar.
		 * @return New quaternion with product values.
		 */
		Quat operator*(float value) const;

		/**
		 * @brief Division operator.
		 * @param Scalar.
		 * @return New quaternion with quotient values.
		 */
		Quat operator/(float value) const;

		/**
		 * @brief Addition operator.
		 * @param Other quaternion.
		 * @return This quaternion.
		 */
		Quat& operator+=(const Quat& other);

		/**
		 * @brief Addition operator.
		 * @param 4D vector.
		 * @return This quaternion
		 */
		Quat& operator+=(const Vec_t<4>& vec);

		/**
		 * @brief Subtraction operator.
		 * @param Other quaternion.
		 * @return This quaternion.
		 */
		Quat& operator-=(const Quat& other);

		/**
		 * @brief Subtraction operator.
		 * @param 4D vector.
		 * @return This quaternion
		 */
		Quat& operator-=(const Vec_t<4>& vec);

		/**
		 * @brief Multiplication operator.
		 * @param Other quaternion.
		 * @return This quaternion.
		 */
		Quat& operator*=(const Quat& other);

		/**
		 * @brief Multiplication operator.
		 * @param 4D vector.
		 * @return This quaternion
		 */
		Quat& operator*=(const Vec_t<4>& vec);

		/**
		 * @brief Multiplication operator.
		 * @param Scalar.
		 * @return This quaternion.
		 */
		Quat& operator*=(float value);

		/**
		 * @brief Division operator.
		 * @param Scalar.
		 * @return This quaternion.
		 */
		Quat& operator/=(float value);

		/**
		 * @brief Calculate the square magnitude of the quaternion.
		 * @return Square magnitude.
		 */
		float square_magnitude() const;

		/**
		 * @brief Calculate the magnitude of the quaternion.
		 * @return Magnitude.
		 */
		float magnitude() const;

		/**
		 * @brief Normalize the quaternion.
		 * @return This quaternion.
		 */
		Quat& normalize();

		/**
		 * @brief Create a normalized version of this quaternion.
		 * @return New quaternion that has been normalized.
		 */
		Quat normalized() const;

		/**
		 * @brief Dot product.
		 * @param Other quaternion.
		 * @return Result of the dot product.
		 */
		float dot(const Quat& other) const;

		/**
		 * @brief Dot product.
		 * @param 4D vector.
		 * @return Result of the dot product.
		 */
		float dot(const Vec_t<4>& vec) const;

		/**
		 * @brief Convert euler angles into a quaternion.
		 * @param Euler angles.
		 * @return This quaternion.
		 */
		Quat& euler_angles(const Vec_t<3>& euler);

		/**
		 * @brief Convert the quaternion into a 4x4 matrix.
		 * @return 4x4 matrix representation of the quaternion.
		 */
		Mat_t<4, 4> as_matrix() const;

		/**
		 * @brief Convert the quaternion into euler angles.
		 * @param Quaternion as euler angles.
		 */
		Vec_t<3> as_euler() const;



		/** Quaternion data */
		union
		{
			struct { float x, y, z, w; };
			float data[4];
		};
	};



	/**
	 * @brief Compute the angle between two quaternions.
	 * @param First quaternion.
	 * @param Second quaternion.
	 * @return Angle between the two quaternions.
	 */
	float angle(const Quat& q1, const Quat& q2);

	/**
	 * @brief Lerp between two quaternions and normalize the result.
	 * @param First quaternion.
	 * @param Second quaternion.
	 * @param Interpolant.
	 * @note The interpolant will be clamped between 0 and 1.
	 * @return Lerped quaternion.
	 */
	Quat lerp(const Quat& q1, const Quat& q2, float interpolant);

	/**
	 * @brief Spherically interpolant between two quaternions and normalize the result.
	 * @param First quaternion.
	 * @param Second quaternion.
	 * @param Interpolant.
	 * @note The interpolant will be clamped between 0 and 1.
	 * @return Slerped quaternion.
	 */
	Quat slerp(const Quat& q1, const Quat& q2, float interpolant);
}