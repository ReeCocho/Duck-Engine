#pragma once

/**
 * @file scene.hpp
 * @brief ECS scene.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "entity.hpp"
#include "component.hpp"
#include "system.hpp"

namespace dk
{
	/**
	 * @brief ECS scene.
	 */
	class Scene
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Scene();

		/**
		 * @brief Shutdown the scene.
		 */
		void shutdown();

	private:
	};
}