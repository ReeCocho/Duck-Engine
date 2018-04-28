/**
 * @file frustum.cpp
 * @brief Frustum source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "debugging.hpp"
#include "frustum.hpp"

namespace dk
{
	Frustum::Frustum(glm::mat4 mat)
	{
		// Compute plane equations
		planes[0] = glm::vec4(mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], mat[3][3] - mat[3][0]);
		planes[1] = glm::vec4(mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], mat[3][3] + mat[3][0]);
		planes[2] = glm::vec4(mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], mat[3][3] + mat[3][1]);
		planes[3] = glm::vec4(mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], mat[3][3] - mat[3][1]);
		planes[4] = glm::vec4(mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], mat[3][3] - mat[3][2]);
		planes[5] = glm::vec4(mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], mat[3][3] + mat[3][2]);
	}

	float Frustum::distance(size_t i, glm::vec3 p)
	{
		dk_assert(i < 6);
		glm::vec3 norm = glm::vec3(planes[i]);
		return (glm::dot(norm, p) + planes[i].w) / glm::length(norm);
	}

	bool Frustum::check_inside(glm::vec3 p)
	{
		// Loop over every plane and check if
		// the point is in front or behind it.
		for (size_t i = 0; i < 6; ++i)
			if (distance(i, p) > 0.0f) return false;

		return true;
	}

	bool Frustum::check_inside(const AABB& aabb)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			float d = glm::dot(aabb.center, glm::vec3(planes[i]));
			float r = glm::dot(aabb.extent, glm::abs(glm::vec3(planes[i])));
			if (d + r < -planes[i].w) return false;
		}

		return true;
	}

	bool Frustum::check_inside(glm::vec3 c, float r)
	{
		float dist01 = glm::min(distance(0, c), distance(1, c));
		float dist23 = glm::min(distance(2, c), distance(3, c));
		float dist45 = glm::min(distance(4, c), distance(5, c));
		return glm::min(glm::min(dist01, dist23), dist45) + r;
	}
}