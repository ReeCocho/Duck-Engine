/**
 * @file component.imp.hpp
 * @brief Component header implementation.
 * @author Connor J. Bramham (ReeCocho)
 */

namespace dk
{
	template<class T>
	Component<T>::Component() :
		m_system(nullptr),
		m_entity({})
	{}

	template<class T>
	Component<T>::Component(System<T>* system, const Entity& entity) :
		m_system(system),
		m_entity(entity)
	{}

	template<class T>
	Entity Component<T>::get_entity() const
	{
		return m_entity;
	}

	template<class T>
	System<T>& Component<T>::get_system() const
	{
		return m_system;
	}

	template<class T>
	Scene& Component<T>::get_scene() const
	{
		return m_entity.get_scene();
	}

	template<class T>
	Handle<T> Component<T>::get_handle() const
	{
		return m_entity.get_component<T>();
	}
}