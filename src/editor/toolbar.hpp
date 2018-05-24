#pragma once

/**
 * @file toolbar.hpp
 * @brief Tool bar header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <graphics\graphics.hpp>
#include <ecs\scene.hpp>
#include "imgui\imgui.h"

namespace dk
{
	/**
	 * Toolbar utility
	 */
	class Toolbar
	{
	public:

		/**
		 * Constructor.
		 * @param Graphics context.
		 * @param Scene.
		 */
		Toolbar(Graphics* graphics, Scene* scene);

		/**
		 * Destructor.
		 */
		~Toolbar() = default;

		/**
		 * Draw the toolbar.
		 */
		void draw();

	private:

		/** Graphics context. */
		Graphics* m_graphics;

		/** Scene. */
		Scene* m_scene;
	};
}