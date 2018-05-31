#pragma once

/**
 * @file inspector.hpp
 * @brief Inspector header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <graphics\graphics.hpp>
#include <components\transform.hpp>
#include <ecs\scene.hpp>
#include "imgui\imgui.h"

namespace dk
{
	/**
	 * Inspector widget.
	 */
	class Inspector
	{
	public:

		/**
		 * Constructor.
		 * @param Graphics context.
		 * @param Scene to inspect.
		 */
		Inspector(Graphics* graphics, Scene* scene);

		/**
		 * Destructor.
		 */
		~Inspector() = default;

		/**
		 * Draw the widget.
		 */
		void draw();

		/**
		 * Change which entity is being inspected.
		 * @param Entity to inspect.
		 */
		void inspect_entity(Entity entity)
		{
			m_inspected_entity = entity;
		}

	private:

		/** Graphics context. */
		Graphics* m_graphics;

		/** Scene. */
		Scene* m_scene;

		/** Entity being inspected. */
		Entity m_inspected_entity = Entity(nullptr, 0);
	};
}