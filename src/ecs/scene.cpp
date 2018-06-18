/**
 * @file scene.cpp
 * @brief Scene source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <functional>
#include <utilities\debugging.hpp>
#include <engine\config.hpp>
#include "system.hpp"
#include "scene.hpp"

namespace dk
{
	Scene::Scene(entity_id entity_counter, const std::vector<entity_id>& free_ids) :
		m_entity_id_counter(entity_counter),
		m_free_entity_ids(free_ids)
	{}

	void Scene::shutdown()
	{
		// Loop over every entity ID
		for (entity_id entity = 1; entity <= m_entity_id_counter; ++entity)
		{
			// Only continue if the entity is not in the free id vector
			if (std::find(m_free_entity_ids.begin(), m_free_entity_ids.end(), entity) != m_free_entity_ids.end())
				continue;

			// Destroy the entity
			const Entity e = Entity(this, entity);
			destroy_entity(e);
		}

		// Destroy systems
		m_systems.clear();
	}

	void Scene::tick(float dt)
	{
		// Loop over every system and call on_tick(), on_late_tick(), 
		// and on_pre_render() in that order

		for (auto& system : m_systems)
#if DK_EDITOR
			if (system->runs_in_editor())
#endif
			{
				system->on_tick(dt);
			}

		for (auto& system : m_systems)
#if DK_EDITOR
			if (system->runs_in_editor())
#endif
			{
				system->on_late_tick(dt);
			}

		for (auto& system : m_systems)
#if DK_EDITOR
			if (system->runs_in_editor())
#endif
			{
				system->on_pre_render(dt);
			}
	}

	ISystem* Scene::get_system_by_id(type_id component_id)
	{
		// Loop over every system
		for (auto& system : m_systems)
		{
			// If the system works with the requested component, we found it
			if (system->get_component_type() == component_id)
				return system.get();
		}

		// Return nullptr if no system operating on the requested type was found
		return nullptr;
	}

	ISystem* Scene::get_system_by_name(const std::string& name)
	{
		// Loop over every system
		for (auto& system : m_systems)
			// If the system has the name we want, return it
			if (system->get_name() == name)
				return system.get();

		// Return nullptr if no system was found
		return nullptr;
	}

	Entity Scene::create_entity()
	{
		// New entity ID
		entity_id id = 0;

		// Check if there are any free entity handles or not
		if (m_free_entity_ids.empty())
			id = ++m_entity_id_counter;
		else
		{
			// Get the new entity id and remove it from the free id queue
			id = m_free_entity_ids.front();
			m_free_entity_ids.erase(m_free_entity_ids.begin());
		}

		// Run on_new_entity() for every system
		for (auto& system : m_systems)
			system->on_new_entity(Entity(this, id));

		return Entity(this, id);
	}

	void Scene::destroy_entity(const Entity& entity)
	{
		dk_assert(entity.is_valid() && &entity.get_scene() == this);

		// Add the entity ID to the free id queue
		m_free_entity_ids.push_back(entity.get_id());

		// Loop over every system and destroy the component
		// which belongs to the given entity.
		for (auto& system : m_systems)
			system->remove_component(entity);
	}
}