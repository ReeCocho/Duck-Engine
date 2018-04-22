#pragma once

/**
 * @file engine.hpp
 * @brief Engine header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "graphics\graphics.hpp"
#include "graphics\renderer.hpp"
#include "input.hpp"
#include "ecs\scene.hpp"
#include "resource_manager.hpp"

namespace dk
{
	namespace engine
	{
		/** Graphics context. */
		extern Graphics graphics;

		/** Rendering engine. */
		extern Renderer renderer;

		/** Resource manager. */
		extern ResourceManager resource_manager;

		/** Input manager. */
		extern Input input;

		/** Active scene. */
		extern Scene scene;



		/**
		 * @brief Initialize the engine.
		 * @param Thread count.
		 * @param Name of the window.
		 * @param Width of the window.
		 * @param Height of the window.
		 */
		extern void initialize(size_t thread_count, const std::string& name, int width, int height);

		/**
		 * @brief Simulate the game until completion.
		 */
		extern void simulate();

		/**
		 * @brief Shutdown the engine.
		 */
		extern void shutdown();
	}
}