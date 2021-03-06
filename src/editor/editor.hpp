#pragma once

/**
 * @file editor.hpp
 * @brief Editor header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <engine\config.hpp>
#include <graphics\graphics.hpp>
#include <graphics\forward_renderer.hpp>
#include <engine\input.hpp>
#include <ecs\scene.hpp>
#include <physics\physics.hpp>
#include <engine\resource_manager.hpp>
#include "editor_renderer.hpp"

#if DK_EDITOR
namespace dk
{
	namespace editor
	{
		/** Graphics context. */
		extern Graphics graphics;

		/** Scene rendering engine. */
		extern OffScreenForwardRenderer renderer;

		/** Editor rendering engine. */
		extern EditorRenderer editor_renderer;

		/** Resource manager. */
		extern ResourceManager resource_manager;

		/* Physics engine. */
		extern Physics physics;

		/** Input manager. */
		extern Input input;

		/** Active scene. */
		extern Scene scene;



		/**
		 * Initialize the engine.
		 * @param Path to config file.
		 */
		extern void initialize(const std::string& path);

		/**
		 * Simulate the game until completion.
		 */
		extern void simulate();

		/**
		 * Shutdown the engine.
		 */
		extern void shutdown();
	}

	namespace engine = editor;
}
#endif