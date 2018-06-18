#pragma once

/**
 * @file component.hpp
 * @brief A container of data which is associated with an entity and is acted upon by a system.
 * @author Connor J. Bramham (ReeCocho)
 */
/** Includes. */
#include "entity.hpp"

/**
* Defines a contructor and destructor for convenience.
* @param Component type.
*/
#define DK_COMPONENT_BODY(T) \
T() : dk::Component<T>(nullptr, dk::Entity()) {} \
T(dk::System<T>* system, const dk::Entity& e) : dk::Component<T>(system, e) {}

namespace dk
{
	/** Forward declare System<T>. */
	template<class T>
	class System;

	 /**
	  * Container of data which is associated with an entity and is acted upon by a system.
	  * @tparam Derived type of the component.
	  */
	template<class T>
	class Component
	{
	public:

		/**
		 * Default constructor.
		 */
		Component();

		/**
		 * Constructor.
		 * @param System the component belongs to.
		 * @param Entity the component is associated with.
		 */
		Component(System<T>* system, const Entity& entity);

		/**
		 * Destructor.
		 */
		virtual ~Component() = default;

		/**
		 * Get the entity the component belongs to.
		 * @return Entity.
		 */
		inline Entity get_entity() const;

		/**
		 * Get the system the component resides in.
		 * @return System.
		 */
		inline System<T>& get_system() const;

		/**
		 * Get the scene the component's entity is in.
		 * @return Scene.
		 */
		inline Scene& get_scene() const;

		/**
		 * Get the components handle.
		 * @return The components handle.
		 */
		inline Handle<T> get_handle() const;

	private:

		/** Entity the component is associated with. */
		Entity m_entity;

		/** System the component belongs to. */
		System<T>* m_system;
	};
}

#include "component.imp.hpp"