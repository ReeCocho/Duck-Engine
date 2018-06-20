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
#include <engine\resource_manager.hpp>
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
		 * @param Resource manager.
		 */
		Inspector(Graphics* graphics, Scene* scene, ResourceManager* resource_manager);

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
		void inspect_entity(Entity entity);

		/**
		 * Get the inspected entity.
		 * @return The inspected entity.
		 */
		inline Entity get_inspected_entity() const
		{
			return m_inspected_entity;
		}

	private:

		/**
		 * Draw a field.
		 * @param Field.
		 * @return If the field was changed.
		 */
		bool draw_field(ReflectionContext::Field* field);

		/** Graphics context. */
		Graphics* m_graphics;

		/** Scene. */
		Scene* m_scene;

		/** Resource manager. */
		ResourceManager* m_resource_manager;

		/** Entity being inspected. */
		Entity m_inspected_entity = Entity(nullptr, 0);

		/** Systems inspected components and the components ID. */
		std::vector<std::tuple<type_id, std::unique_ptr<ReflectionContext>>> m_component_inspectors = {};
	};
}