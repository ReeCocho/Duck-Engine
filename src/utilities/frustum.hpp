#pragma once

/**
 * @file frustum.hpp
 * @brief Frustum utility file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <array>
#include <glm\glm.hpp>

namespace dk
{
	/**
	 * AABB bounding box.
	 */
	struct AABB
	{
		/** Center */
		glm::vec3 center = {};

		/** Extents */
		glm::vec3 extent = {};

		/**
		 * @brief Transform the AABB by a model matrix.
		 * @param Model matrix.
		 * @return This.
		 */
		AABB& transform(glm::mat4 model);
	};



	/**
	 * A frustum described as a set of 6 plane equations.
	 */
	class Frustum
	{
	public:

		/**
		 * Default constructor.
		 */
		Frustum() = default;

		/**
		 * Create the frustum based on a matrix.
		 * @param Matrix.
		 */
		Frustum(glm::mat4 mat);



		/**
		 * Get the distance from a point to a plane.
		 * @param Plane index.
		 * @param Point.
		 * @return Distance to plane.
		 * @note A negative distance means the point is below the normal.
		 */
		float distance(size_t i, glm::vec3 p);

		/**
		 * Check if a point is inside the frustum.
		 * @param Point.
		 * @return If the point is inside.
		 */
		bool check_inside(glm::vec3 p);

		/**
		 * Check if an AABB bounding box is inside the frustum.
		 * @param AABB bounding box.
		 * @return If the bounding box is inside.
		 */
		bool check_inside(const AABB& aabb);

		/**
		 * Check if a sphere is inside the frustum.
		 * @param Sphere center.
		 * @param Radius.
		 * @return If the sphere is inside.
		 */
		bool check_inside(glm::vec3 c, float r);



		/** Plane equations. */
		std::array<glm::vec4, 6> planes = {};
	};
}