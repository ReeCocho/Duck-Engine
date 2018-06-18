/**
 * @file entity.imp.hpp
 * @brief Entity header implementation file.
 * @author Connor J. Bramham (ReeCocho)
 */

namespace dk
{
	bool Entity::operator==(const Entity& other) const
	{
		return m_scene == other.m_scene && m_id == other.m_id;
	}

	bool Entity::operator!=(const Entity& other) const
	{
		return m_scene != other.m_scene || m_id != other.m_id;
	}

	Scene& Entity::get_scene() const
	{
		return *m_scene;
	}

	entity_id Entity::get_id() const
	{
		return m_id;
	}

	bool Entity::is_valid() const
	{
		return m_scene != nullptr && m_id > 0;
	}

	template<class C>
	inline Handle<C> Entity::get_component() const
	{
		static_assert(std::is_convertible<C, Component<C>>::value, "C must derive from Component<C>.");

		// Get the system which operates on component C
		auto* system = static_cast<System<C>*>(m_scene->get_system_by_id(TypeID<C>::id()));

		// Get the component
		if (system) return system->get_component(*this);
		else return Handle<C>();
	}

	template<class C>
	inline Handle<C> Entity::add_component() const
	{
		static_assert(std::is_convertible<C, Component<C>>::value, "C must derive from Component<C>.");

		// Get the system which operates on component C
		auto* system = static_cast<System<C>*>(m_scene->get_system_by_id(TypeID<C>::id()));

		// Add the component
		if (system)
		{
			system->add_component(*this);
			return system->get_component(*this);
		}
		else return Handle<C>();
	}

	template<class C>
	inline void Entity::remove_component() const
	{
		static_assert(std::is_convertible<C, Component<C>>::value, "C must derive from Component<C>.");

		// Get the system which operates on component C
		auto* system = static_cast<System<C>*>(m_scene->get_system_by_id(TypeID<C>::id()));

		// Remove the component
		if (system) system->remove_component(*this);
	}
}