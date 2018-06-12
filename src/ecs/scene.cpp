/**
 * @file scene.cpp
 * @brief ECS scene source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <engine/config.hpp>
#include "scene.hpp"

namespace dk
{
	Scene::Scene() {}

	void Scene::shutdown()
	{
		for (auto& system : m_systems)
			system->remove_all_components();

		m_systems.clear();
	}

	EntityID Scene::create_entity()
	{
		EntityID handle = 0;

		if (m_free_ids.size() > 0)
		{
			handle = m_free_ids[0];
			m_free_ids.erase(m_free_ids.begin());
		}
		else 
			handle = ++m_entity_id_counter;

		for (auto& system : m_systems)
		{
#if DK_EDITOR
			if (system->get_runs_in_editor())
#endif
				system->on_new_entity(Entity(this, handle));
		}

		return handle;
	}

	void Scene::destroy_entities()
	{
		for (auto entity : m_entities_marked_for_delete)
		{
			for (auto& system : m_systems)
				system->remove_component(Entity(this, entity));

			m_free_ids.push_back(entity);
		}

		m_entities_marked_for_delete.clear();
	}

	void Scene::destroy_components()
	{
		for (auto component : m_components_marked_for_delete)
			for(auto& system : m_systems)
				if (system->get_id() == std::get<1>(component))
					system->remove_component(std::get<0>(component));

		m_components_marked_for_delete.clear();
	}

	void Scene::tick(float delta_time)
	{
		destroy_entities();
		destroy_components();

		for (auto& system : m_systems)
		{
#if DK_EDITOR
			if(system->get_runs_in_editor())
#endif
			system->on_tick(delta_time);
		}

		for (auto& system : m_systems)
		{
#if DK_EDITOR
			if (system->get_runs_in_editor())
#endif
			system->on_late_tick(delta_time);
		}

		for (auto& system : m_systems)
		{
#if DK_EDITOR
			if (system->get_runs_in_editor())
#endif
			system->on_pre_render(delta_time);
		}
	}

	SerializableScene Scene::get_serializable_scene()
	{
		SerializableScene scene = {};
		scene.entity_id_counter = m_entity_id_counter;
		scene.free_entities = m_free_ids;
		scene.systems.resize(m_systems.size());

		for (size_t i = 0; i < m_systems.size(); ++i)
			scene.systems[i] = m_systems[i]->get_serializable_system();

		return scene;
	}

	void Scene::update_entities(EntityID counter, const std::vector<EntityID>& free_ids)
	{
		m_entity_id_counter = counter;
		m_free_ids = free_ids;
	}
}