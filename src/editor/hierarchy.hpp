#pragma once

/**
 * @file hierachy.hpp
 * @brief Hierarchy header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <graphics\graphics.hpp>
#include <ecs\transform.hpp>
#include <ecs\scene.hpp>
#include "imgui\imgui.h"

namespace dk
{
	/**
	 * Hierarchy widget.
	 */
	class EditorHierarchy
	{
	public:

		/**
		 * Constructor.
		 * @param Graphics context.
		 * @param Scene to inspect.
		 */
		EditorHierarchy(Graphics* graphics, Scene* scene);

		/**
		 * Destructor.
		 */
		~EditorHierarchy() = default;

		/**
		 * Draw the widget.
		 */
		void draw();

	private:

		/**
		 * Recursivley create a tree structure based off a transform.
		 * @param Transform handle.
		 */
		void draw_transform_tree(Handle<Transform> transform);



		/** Graphics context. */
		Graphics* m_graphics;

		/** Scene. */
		Scene* m_scene;

		/** Transform system. */
		System<Transform>* m_transform_system;
	};
}