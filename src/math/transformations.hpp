#pragma once

/**
 * @file transformations.hpp
 * @brief Matrix transformation utilities.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vectors.hpp"
#include "matrices.hpp"
#include "quaternions.hpp"

namespace dk
{
	/**
	 * @brief Create a perspective matrix.
	 * @param Near clipping plane.
	 * @param Far clipping plane.
	 * @param Aspect ratio.
	 * @param Field of view.
	 * @return The resulting perspective matrix.
	 */
	Mat_t<4, 4> perspective(float near, float far, float aspect, float fov);

	/**
	 * @brief Create a orthographic projection matrix.
	 * @param Near clipping plane.
	 * @param Far clipping plane.
	 * @param Width.
	 * @param Height.
	 * @return Resulting projection matrix.
	 */
	Mat_t<4, 4> ortho(float near, float far, float w, float h);

	/**
	 * @brief A transformation matrix corresponding to a camera viewing a point.
	 * @param Source.
	 * @param Target.
	 * @param Up.
	 * @return Resulting transformation matrix.
	 */
	Mat_t<4, 4> look_at(const Vec_t<3>& src, const Vec_t<3>& trg, const Vec_t<3>& up);

	/**
	 * @brief Create a translation matrix.
	 * @param Position.
	 * @return Translation matrix.
	 */
	Mat_t<4, 4> translate(const Vec_t<4>& pos);

	/**
	 * @brief Create a rotation matrix.
	 * @param Quaternion rotation.
	 * @return Resulting rotation matrix.
	 */
	Mat_t<4, 4> rotate(const Quat& rot);

	/**
	 * @brief Create a scaling matrix.
	 * @param Scale.
	 * @return Resulting scaling matrix.
	 */
	Mat_t<4, 4> scale(const Vec_t<4>& scl);
}