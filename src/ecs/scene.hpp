#pragma once

/**
 * @file scene.hpp
 * @brief ECS scene.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <memory>
#include <tuple>
#include "entity.hpp"
#include "component.hpp"
#include "system.hpp"

namespace dk
{
	/**
	 * ECS scene.
	 */
	class Scene
	{
	public:

		/**
		 * Constructor.
		 */
		Scene();

		/**
		 * Shutdown the scene.
		 */
		void shutdown();

		/**
		 * Perform a tick in the scene.
		 * @param Time in seconds since the last frame.
		 */
		void tick(float delta_time);

		/**
		 * Save/load the entire scene depending on whether the archive is in read or write mode.
		 * @param Generic archive.
		 * @param Archive for components.
		 * @note The component archive should use the generic archive to read and write.
		 */
		void serialize(Archive& archive, ReflectionContext& comp_archive);

		/**
		 * Create a new entity.
		 * @return New entity ID.
		 */
		EntityID create_entity();

		/**
		 * Add a system.
		 * @tparam Type of system.
		 */
		template<class T>
		void add_system()
		{
			if(!get_system_by_base(TypeID<T>::id()))
				m_systems.push_back(std::make_unique<T>(this));
		}

		/**
		 * Add a component.
		 * @tparam Type of component.
		 * @param Entity the component will belong to.
		 * @return Component handle.
		 * @note Will not create a new component if one already exists for the given entity.
		 */
		template<class T>
		Handle<T> add_component(Entity entity)
		{
			static_assert(std::is_convertible<T, Component<T>>::value, "T must derive from Component<T>.");

			System<T>* system = static_cast<System<T>*>(get_system_by_base(TypeID<T>::id()));
			dk_assert(system);

			// Make sure the component does not already exist
			Handle<T> component = system->find_component_by_entity(entity);
			if (component.allocator != nullptr)
				return component;

			return system->add_component(entity);
		}

		/**
		 * Get a component from an entity.
		 * @tparam Type of component.
		 * @param Entity the component belongs to.
		 * @return Component handle.
		 * @note Will return a Handle<T>() if the entity does not contain the component.
		 */
		template<class T>
		Handle<T> get_component(Entity entity)
		{
			static_assert(std::is_convertible<T, Component<T>>::value, "T must derive from Component<T>.");

			System<T>* system = static_cast<System<T>*>(get_system_by_base(TypeID<T>::id()));
			dk_assert(system);

			return system->find_component_by_entity(entity);
		}

		/**
		 * Get number of systems.
		 * @return Number of systems.
		 */
		inline size_t get_system_count() const
		{
			return m_systems.size();
		}

		/**
		 * Get a system
		 * @tparam Type of component the system manages.
		 * @return System.
		 */
		template<class T>
		inline System<T>* get_system()
		{
			SystemBase* system = get_system_by_base(TypeID<T>::id());
			dk_assert(system);
			return static_cast<System<T>*>(system);
		}

		/**
		 * Find a system belonging to a specific type.
		 * @param Type of component the system manages.
		 * @return Pointer to system.
		 * @return Will be nullptr if the system doesn't exist.
		 */
		inline SystemBase* get_system_by_base(size_t id)
		{
			for (size_t i = 0; i < m_systems.size(); ++i)
				if (m_systems[i]->get_id() == id)
					return m_systems[i].get();

			return nullptr;
		}

		/**
		 * Find a system by it's index.
		 * @param System index.
		 * @return Pointer to system.
		 * @note Will be nullptr if the system doesn't exist.
		 */
		inline SystemBase* get_system_by_index(size_t i)
		{
			dk_assert(i < m_systems.size());
			return m_systems[i].get();
		}

		/**
		 * Find system by it's name.
		 * @param System name.
		 * @return Pointer to system.
		 * @note Will be nullptr if the system doesn't exist.
		 */
		inline SystemBase* get_system_by_name(const std::string& name)
		{
			for (auto& system : m_systems)
				if (system->get_name() == name)
					return system.get();
			return nullptr;
		}

		/**
		 * Remove a component from an entity.
		 * @tparam Type of component.
		 * @param entity the component belongs to.
		 */
		template<class T>
		inline void remove_component(Entity entity)
		{
			static_assert(std::is_convertible<T, Component<T>>::value, "T must derive from Component<T>.");
			m_components_marked_for_delete.push_back(std::make_tuple(entity, TypeID<T>::id()));
		}

	private:

		/**
		 * @brief Destroy every marked entity.
		 */
		void destroy_entities();

		/**
		 * @brief Destroy every marked component.
		 */
		void destroy_components();

		/** Systems. */
		std::vector<std::unique_ptr<SystemBase>> m_systems = {};

		/** Entity ID counter. */
		EntityID m_entity_id_counter = 0;

		/** List of free entity IDs. */
		std::vector<EntityID> m_free_ids = {};

		/** List of entities to be destroyed next tick. */
		std::vector<EntityID> m_entities_marked_for_delete = {};

		/** List of components entity ID to be destroyed next tick. */
		std::vector<std::tuple<Entity, ResourceID>> m_components_marked_for_delete = {};
	};

	template<class T>
	Handle<T> Entity::add_component()
	{
		return m_scene->add_component<T>(*this);
	}

	template<class T>
	Handle<T> Entity::get_component()
	{
		return m_scene->get_component<T>(*this);
	}

	template<class T>
	void Entity::remove_component()
	{
		m_scene->remove_component<T>(*this);
	}
}