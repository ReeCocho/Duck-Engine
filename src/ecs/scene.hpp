#pragma once

/**
 * @file scene.hpp
 * @brief Manages entities and systems.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <vector>
#include <queue>
#include <memory>
#include "entity.hpp"
#include "system.hpp"
#include "component.hpp"

namespace dk
{
	/**
	 * Scene which can be serialized.
	 */
	struct SerializableScene
	{
		/** Entity ID counter. */
		entity_id entity_counter = 0;

		/** Free entity IDs. */
		std::vector<entity_id> free_entity_ids = {};

		/** Systems to serialize. */
		std::vector<ISystem*> systems = {};
	};

	/** 
	 * Manages entities and systems. 
	 */
	class Scene
	{
	public:

		/**
		 * Constructor.
		 */
		Scene() = default;

		/**
		 * Constructor.
		 * @param Entity ID counter.
		 * @param Free entity IDs.
		 */
		Scene(entity_id entity_counter, const std::vector<entity_id>& free_ids);

		/**
		 * Shudown the scene.
		 */
		void shutdown();

		/**
		 * Perform a tick in the system.
		 * @param Time since the last tick.
		 */
		void tick(float dt);

		/**
		 * Get the number of systems.
		 * @return The number of systems.
		 */
		inline size_t get_system_count() const;

		/**
		 * Get a system by it's index.
		 * @param Index of the system.
		 * @return System.
		 */
		inline ISystem& get_system_by_index(size_t i);

		/**
		 * Get a system by the ID of the component it works with.
		 * @param Type ID of the component the system works with.
		 * @return System.
		 * @note Returns nullptr if a system that operates on the requested type doesn't exist.
		 */
		ISystem* get_system_by_id(type_id component_id);

		/**
		 * Get a system by its name.
		 * @param System name.
		 * @return System.
		 * @note Will return nullptr if a system does not exist with the given name.
		 */
		ISystem* get_system_by_name(const std::string& name);

		/**
		 * Add a system to the scene.
		 * @tparam System type.
		 */
		template<class T>
		void add_system();

		/**
		 * Create a new entity.
		 * @return Entity.
		 */
		Entity create_entity();

		/**
		 * Destroy an entity.
		 * @param Entity to destroy.
		 */
		void destroy_entity(const Entity& entity);

		/**
		 * Create a serializable scene for this scene.
		 * @return Serializable scene.
		 */
		inline SerializableScene get_serializable_scene() const;

		/**
		 * Update entity values.
		 * @param Entity counter.
		 * @param Free entity ids.
		 * @note No entities must have been created for this to succeed.
		 */
		inline void update_entities(entity_id counter, const std::vector<entity_id>& free_ids);

	private:

		/** Systems. */
		std::vector<std::unique_ptr<ISystem>> m_systems = {};

		/** Counter for entity ids. */
		entity_id m_entity_id_counter = 0;

		/** Free entity ids. */
		std::vector<entity_id> m_free_entity_ids = {};
	};
}

#include "scene.imp.hpp"