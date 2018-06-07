#pragma once

/**
 * @file component.hpp
 * @brief ECS component.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\reflection.hpp>
#include "entity.hpp"

/** 
 * Defines constructors for a component. 
 * @param Component type.
 */
#define DK_COMPONENT_BODY(T) \
T() : dk::Component<T>(dk::Handle<T>(0, nullptr), dk::Entity()) {} \
T(dk::Handle<T> handle, dk::Entity entity) : dk::Component<T>(handle, entity) {}

namespace dk
{
	/**
	 * ECS component.
	 * @tparam Type of the component.
	 */
	template<class T>
	class Component
	{
	public:

		/**
		 * Constructor.
		 * @param Components handle.
		 * @param Entity the component belongs to.
		 */
		Component(Handle<T> handle, Entity entity) : 
			m_handle(handle), 
			m_entity(entity)
		{}

		/**
		 * Destructor.
		 */
		virtual ~Component() {}
		
		/**
		 * Get the scene the component exists in.
		 * @return Scene.
		 */
		inline Scene& get_scene() const
		{
			return m_entity.get_scene();
		}

		/**
		 * Get the ID of the component.
		 * @return ID.
		 */
		constexpr type_id get_id() const
		{
			return TypeID<T>::id();
		}

		/**
		 * Get the entity the component belongs to.
		 * @return Entity.
		 */
		inline Entity get_entity() const
		{
			return m_entity;
		}

		/**
		 * Get this components handle.
		 * @return Components handle.
		 */
		inline Handle<T> get_handle() const
		{
			return m_handle;
		}

	private:

		/** Entity the component belongs to. */
		Entity m_entity;

		/** Components handle */
		Handle<T> m_handle;
	};
}