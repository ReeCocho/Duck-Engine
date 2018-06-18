/**
 * @file system.imp.hpp
 * @brief System header implementation file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\debugging.hpp>

namespace dk
{
	type_id ISystem::get_component_type() const
	{
		return m_component_type;
	}

	bool ISystem::runs_in_editor() const
	{
		return m_runs_in_editor;
	}

	Scene& ISystem::get_scene() const
	{
		return *m_scene;
	}

	std::string ISystem::get_name() const
	{
		return m_name;
	}

	void ISystem::set_active_component(resource_id component_id)
	{
		m_active_component = component_id;
	}

	template<class C>
	System<C>::iterator::iterator(System* system, resource_id start_component) :
		m_system(system),
		m_component(start_component)
	{}

	template<class C>
	inline typename System<C>::iterator& System<C>::iterator::operator++()
	{
		// Increment the active component
		++m_component;

		// While we haven't reached the end of the component allocator and we haven't found an allocated component...
		while (m_component != static_cast<resource_id>(m_system->m_allocator.max_allocated()) &&
			!m_system->m_allocator.is_allocated(m_component))
			// Increment the active component
			++m_component;

		// Update the active component
		m_system->m_active_component = m_component;

		return *this;
	}

	template<class C>
	inline bool System<C>::iterator::operator!=(const iterator& other) const
	{
		// Compare the system and active component
		return m_component != other.m_component;
	}

	template<class C>
	inline Handle<C> System<C>::iterator::operator*() const
	{
		// Just get the active component
		return Handle<C>(m_component, &m_system->m_allocator);
	}

	template<class C>
	System<C>::System(Scene* scene, const std::string& name, bool runs_in_editor) : ISystem(scene, name, TypeID<C>::id(), runs_in_editor), m_allocator(1) {}

	template<class C>
	System<C>::~System() {}

	template<class C>
	size_t System<C>::get_component_count() const
	{
		return m_allocator.num_allocated();
	}

	template<class C>
	inline Handle<C> System<C>::get_active_component()
	{
		return Handle<C>(m_active_component, &m_allocator);
	}

	template<class C>
	Handle<C> System<C>::get_component(const Entity& e)
	{
		dk_assert(e.is_valid() && &e.get_scene() == &get_scene());

		// Loop over every component in the allocator
		for (resource_id id = 0; id < m_allocator.max_allocated(); ++id)
			// If the component is allocated...
			if (m_allocator.is_allocated(id))
			{
				// Get the component and it's entity
				const C* component = m_allocator.get_resource_by_handle(id);
				const Entity ce = component->get_entity();

				// Check if the entities are the same, and if they are we found our component
				if (ce == e) return Handle<C>(id, &m_allocator);
			}

		// Return a null handle if the entity does not contain the component
		return Handle<C>();
	}

	template<class C>
	void System<C>::add_component(const Entity& e)
	{
		dk_assert(e.is_valid() && &e.get_scene() == &get_scene());

		// Check if the component already exists.
		{
			const Handle<C> component = get_component(e);
			if (component.is_valid()) return;
		}

		// Check if we need to resize the component allocator
		if (m_allocator.num_allocated() == m_allocator.max_allocated())
			m_allocator.resize(m_allocator.max_allocated() + 8);

		// Allocate a new component
		const resource_id component_id = m_allocator.allocate();

		// Call the components contructor
		::new(m_allocator.get_resource_by_handle(component_id))(C)(this, e);

		// Store the old active component
		const resource_id old_active_component = m_active_component;

		// Set the new active component and call on_begin() for it
		m_active_component = component_id;

#if DK_EDITOR
		if (m_runs_in_editor)
#endif
		{
			on_begin();
		}

		// Restore the old active component
		m_active_component = old_active_component;
	}

	template<class C>
	bool System<C>::has_component(const Entity& e)
	{
		// Loop over every component...
		for (resource_id id = 0; id < m_allocator.max_allocated(); id++)
			// and if the component's entity is e, e has the component
			if (m_allocator.get_resource_by_handle(id)->get_entity() == e)
				return true;

		// Could not find a component with entity e
		return false;
	}

	template<class C>
	void System<C>::remove_component(const Entity& e)
	{
		dk_assert(e.is_valid() && &e.get_scene() == &get_scene());

		// Get the component which belongs to entity e
		const Handle<C> component = get_component(e);

		// Stop if the component is invalid (Entity doesn't have the component)
		if (!component.is_valid()) return;

		// Store the old active component
		const resource_id old_active_component = m_active_component;

		// Set the new active component and call on_end() for it
		m_active_component = component.id;

#if DK_EDITOR
		if (m_runs_in_editor)
#endif
		{
			on_end();
		}

		// Restore the old active component
		m_active_component = old_active_component;

		// Deallocate the component
		m_allocator.deallocate(component.id);
	}

	template<class C>
	std::vector<resource_id> System<C>::get_active_components() const
	{
		// Store allocated components
		std::vector<resource_id> components = {};

		// Loop over every component
		for (resource_id id = 0; id < m_allocator.max_allocated(); ++id)
			// If the component is allocated...
			if (m_allocator.is_allocated(id))
				// Add the component to the list
				components.push_back(id);

		return components;
	}

	template<class C>
	Entity System<C>::get_entity_by_component_by_id(resource_id comp_id)
	{
		dk_assert(comp_id < m_allocator.max_allocated() && m_allocator.is_allocated(comp_id));
		return m_allocator.get_resource_by_handle(comp_id)->get_entity();
	}

	template<class C>
	resource_id System<C>::get_component_id_by_entity(const Entity& e)
	{
		dk_assert(e.is_valid() && &get_scene() == &e.get_scene());

		// Loop over every component
		for (resource_id id = 0; id < m_allocator.max_allocated(); ++id)
			// If the components entity equals the entity we are looking for, we found our component
			if (e == m_allocator.get_resource_by_handle(id)->get_entity())
				return id;

		// Could not find a component
		throw std::runtime_error("Could not find a component with entity e.");

		return 0;
	}

	template<class C>
	ResourceAllocatorBase& System<C>::get_component_allocator()
	{
		return m_allocator;
	}

	template<class C>
	void System<C>::load_component(resource_id id, const Entity& e, std::function<void(ReflectionContext&)>& load)
	{
		dk_assert(e.is_valid() && &e.get_scene() == &get_scene());

		// Check if the component already exists.
		{
			const Handle<C> component = get_component(e);
			if (component.is_valid()) return;
		}

		// Check if we need to resize the component allocator
		if (id >= m_allocator.max_allocated())
			m_allocator.resize(id + 1);

		// Allocate a new component
		 m_allocator.allocate_by_id(id);

		// Call the components contructor
		::new(m_allocator.get_resource_by_handle(id))(C)(this, e);

		// Store the old active component
		const resource_id old_active_component = m_active_component;

		// Set the new active component and serialize it
		m_active_component = id;

		ReflectionContext r = {};
		serialize(r);

		// Load the component
		load(r);

		// Run on_begin()
#if DK_EDITOR
		if (m_runs_in_editor)
#endif
		{
			on_begin();
		}

		// Restore the old active component
		m_active_component = old_active_component;
	}

	template<class C>
	inline typename System<C>::iterator System<C>::begin()
	{
		// Loop over the components and find the first allocated one
		resource_id component = 0;
		for (component; component <= m_allocator.max_allocated(); ++component)
			if (component == m_allocator.max_allocated() || m_allocator.is_allocated(component))
				break;

		// Create the iterator
		return iterator(this, component);
	}

	template<class C>
	inline typename System<C>::iterator System<C>::end()
	{
		return iterator(this, static_cast<resource_id>(m_allocator.max_allocated()));
	}
}