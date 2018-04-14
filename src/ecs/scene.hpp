#pragma once

/**
 * @file scene.hpp
 * @brief ECS scene.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <vector>
#include <memory>
#include <tuple>
#include "entity.hpp"
#include "component.hpp"
#include "system.hpp"

namespace dk
{
	class Transform;



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

		/**
		 * @brief Perform a tick in the scene.
		 * @param Time in seconds since the last frame.
		 */
		void tick(float delta_time);

		/**
		 * @brief Create a new entity.
		 * @return New entity ID.
		 */
		size_t create_entity();

		/**
		 * @brief Add a system.
		 * @tparam Type of system.
		 */
		template<class T>
		void add_system()
		{
			auto system = std::make_unique<T>(this);

			if(!find_system(system->get_id()))
				m_systems.push_back(std::move(system));
		}

		/**
		 * @brief Add a component.
		 * @tparam Type of component.
		 * @param Entity the component will belong to.
		 * @return Component handle.
		 * @note Will not create a new component if one already exists for the given entity.
		 */
		template<class T>
		Handle<T> add_component(Entity entity)
		{
			static_assert(std::is_base_of<Component<T>, T>::value, "T must derive from Component<T>.");

			System<T>* system = static_cast<System<T>*>(find_system(TypeID<T>::id()));
			dk_assert(system);

			// Make sure the component does not already exist
			Handle<T> component = system->find_component_by_entity(entity);
			if (component.allocator != nullptr)
				return component;

			return system->add_component(entity);
		}

		/**
		 * @brief Get a component from an entity.
		 * @tparam Type of component.
		 * @param Entity the component belongs to.
		 * @return Component handle.
		 * @note Will return a Handle<T>() if the entity does not contain the component.
		 */
		template<class T>
		Handle<T> get_component(Entity entity)
		{
			static_assert(std::is_base_of<Component<T>, T>::value, "T must derive from Component<T>.");

			System<T>* system = static_cast<System<T>*>(find_system(TypeID<T>::id()));
			dk_assert(system);

			return system->find_component_by_entity(entity);
		}

		/**
		 * @brief Remove a component from an entity.
		 * @tparam Type of component.
		 * @param entity the component belongs to.
		 */
		template<class T>
		void remove_component(Entity entity)
		{
			static_assert(std::is_base_of<Component<T>, T>::value, "T must derive from Component<T>.");
			static_assert(!std::is_same<T, Transform>::value, "T must not be of type dk::Transform");
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

		/**
		 * @brief Find a system belonging to a specific type.
		 * @param Type of component the system manages.
		 * @return Pointer to system.
		 * @return Will be nullptr if the system doesn't exist.
		 */
		SystemBase* find_system(size_t id)
		{
			for (size_t i = 0; i < m_systems.size(); ++i)
				if (m_systems[i]->get_id() == id)
					return m_systems[i].get();

			return nullptr;
		}

		/** Systems. */
		std::vector<std::unique_ptr<SystemBase>> m_systems = {};

		/** Entity ID counter. */
		size_t m_entity_id_counter = 0;

		/** List of free entity IDs. */
		std::vector<size_t> m_free_ids = {};

		/** List of entities to be destroyed next tick. */
		std::vector<size_t> m_entities_marked_for_delete = {};

		/** List of components entity ID to be destroyed next tick. */
		std::vector<std::tuple<Entity, size_t>> m_components_marked_for_delete = {};
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