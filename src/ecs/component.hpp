#pragma once

/**
 * @file component.hpp
 * @brief ECS component.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\reflection.hpp>
#include "entity.hpp"

/** Defines constructors for a component. */
#define DK_COMPONENT_BODY(T) \
T() : dk::Component<T>(dk::Handle<T>(0, nullptr), dk::Entity()) {} \
T(dk::Handle<T> handle, dk::Entity entity) : dk::Component<T>(handle, entity) {}

namespace dk
{
	/**
	 * @brief ECS component.
	 * @tparam Type of the component.
	 */
	template<class T>
	class Component
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Components handle.
		 * @param Entity the component belongs to.
		 */
		Component(Handle<T> handle, Entity entity) : 
			m_handle(handle), 
			m_entity(entity)
		{

		}

		/**
		 * @brief Destructor.
		 */
		virtual ~Component()
		{

		}
		
		/**
		 * @brief Get the scene the component exists in.
		 * @return Scene.
		 */
		Scene& get_scene()
		{
			return m_entity.get_scene();
		}

		/**
		 * @brief Get the ID of the component.
		 * @return ID.
		 */
		size_t get_id() const
		{
			return TypeID<T>::id();
		}

		/**
		 * @brief Get the entity the component belongs to.
		 * @return Entity.
		 */
		Entity get_entity() const
		{
			return m_entity;
		}

		/**
		 * @brief Get this components handle.
		 * @return Components handle.
		 */
		Handle<T> get_handle() const
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