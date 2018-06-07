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

	void Scene::serialize(Archive& archive, ReflectionContext& comp_archive)
	{
		if (archive.is_writing())
		{
			// Write the entity ID counter
			archive.write<EntityID>(m_entity_id_counter);
			
			// Write the free entity ID's
			archive.write<uint32_t>(static_cast<uint32_t>(m_free_ids.size()));

			for (EntityID id : m_free_ids)
				archive.write<EntityID>(id);

			// Write the number of systems
			archive.write<uint32_t>(static_cast<uint32_t>(m_systems.size()));

			// Loop over every system
			for (auto& system : m_systems)
			{
				// Write the system name
				archive.write<std::string>(system->get_name());

				// Serialize the system
				system->serialize_system(archive, comp_archive);
			}
		}
		else
		{
			// Read the entity ID counter
			m_entity_id_counter = archive.read<EntityID>();

			// Read the free entity ID's
			m_free_ids.resize(archive.read<uint32_t>());

			for (size_t i = 0; i < m_free_ids.size(); ++i)
				m_free_ids[i] = archive.read<EntityID>();

			// Read the number of systems
			uint32_t sys_count = archive.read<uint32_t>();
			dk_assert(sys_count == m_systems.size());

			// Call on_new_entity() for each system and each entity
			for (EntityID id = 0; id < m_entity_id_counter; ++id)
			{
				// Make sure the entity isn't deleted
				if (std::find(m_free_ids.begin(), m_free_ids.end(), id) != m_free_ids.end())
					continue;

				// Loop over every sytem
				for (auto& system : m_systems)
				{
#if DK_EDITOR
					if (system->get_runs_in_editor())
#endif
						system->on_new_entity(Entity(this, id + 1));
				}
			}

			// Loop over every system
			for (uint32_t i = 0; i < sys_count; ++i)
			{
				// Read the system name
				std::string sys_name = archive.read<std::string>();

				// Find the system
				SystemBase* system = get_system_by_name(sys_name);
				dk_assert(system);

				// Deserialize the system
				system->serialize_system(archive, comp_archive);
			}
		}
	}
}