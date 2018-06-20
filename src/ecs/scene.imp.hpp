/**
 * @file scene.imp.hpp
 * @brief Scene header implementation file.
 * @author Connor J. Bramham (ReeCocho)
 */

namespace dk
{
	inline size_t Scene::get_system_count() const
	{
		return m_systems.size();
	}

	inline ISystem& Scene::get_system_by_index(size_t i)
	{
		return *m_systems[i];
	}

	template<class T>
	void Scene::add_system()
	{
		// Create the system
		auto system = std::make_unique<T>(this);

		// Check if a system already exists that operates
		// on the same type as the new system
		if (!get_system_by_id(system->get_component_type()))
			m_systems.push_back(std::move(system));
	}

	inline SerializableScene Scene::get_serializable_scene() const
	{
		// Create serializable scene
		SerializableScene serial_scene = {};
		serial_scene.entity_counter = m_entity_id_counter;
		serial_scene.free_entity_ids = m_free_entity_ids;

		// Create system pointer vector
		serial_scene.systems.resize(m_systems.size());
		for (size_t i = 0; i < serial_scene.systems.size(); ++i)
			serial_scene.systems[i] = m_systems[i].get();

		return serial_scene;
	}

	inline bool Scene::entity_exists(const Entity& entity) const
	{
		return	&entity.get_scene() == this &&																				// Scene is the same
				entity.get_id() > 0 &&																						// ID is greater than min
				entity.get_id() <= m_entity_id_counter &&																	// ID is less than max
				std::find(m_free_entity_ids.begin(), m_free_entity_ids.end(), entity.get_id()) == m_free_entity_ids.end();	// Not deleted
	}

	inline void Scene::update_entities(entity_id counter, const std::vector<entity_id>& free_ids)
	{
		dk_assert(m_entity_id_counter == 0 && m_free_entity_ids.size() == 0);
		m_entity_id_counter = counter;
		m_free_entity_ids = free_ids;
	}
}