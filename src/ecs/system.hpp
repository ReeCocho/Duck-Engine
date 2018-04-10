#pragma once

/**
 * @file system.hpp
 * @brief ECS system.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include "component.hpp"

namespace dk
{
	/**
	 * @brief ECS system base.
	 */
	class SystemBase
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 * @param ID of the component the system works with.
		 */
		SystemBase(Scene* scene, size_t id);

		/**
		 * @brief Destructor.
		 */
		virtual ~SystemBase();

		/**
		 * @brief Get the scene.
		 * @return The scene.
		 */
		Scene* get_scene() const
		{
			return m_scene;
		}

		/**
		 * @brief Get the ID of the component the system works with.
		 * @return Component ID.
		 */
		size_t get_id() const
		{
			return m_component_id;
		}

		/**
		 * @brief Called when a component is added to the system.
		 */
		virtual void on_begin();

		/**
		 * @brief Called once per game tick.
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_tick();

		/**
		 * @brief Called after on_tick().
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_late_tick();

		/**
		 * @brief Called after on_late_tick() and before rendering.
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_pre_render();

		/**
		 * @brief Called when a component is removed from the system.
		 */
		virtual void on_end();

	private:

		/** Scene the system exists in. */
		Scene* m_scene;

		/** ID of the component the system works with. */
		size_t m_component_id;
	};

	/**
	 * @brief Base class for user defined systems.
	 * @tparam Type of component the system works with.
	 */
	template<class T>
	class System : public SystemBase
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 * @param Number of components preallocated.
		 */
		System(Scene* scene, size_t pre_alloc = 32) : SystemBase(scene, TypeID<T>.id()), m_components(pre_alloc)
		{

		}

		/**
		 * @brief Destructor.
		 */
		virtual ~System()
		{
			for(size_t i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
					m_active_component = static_cast<ResourceID>(i);
					on_end();
					m_components.deallocate(i);
				}
		}

		/**
		 * @brief Add a component to the system.
		 * @return Components handle.
		 */
		Handle<T> add_component()
		{
			// Resize allocator if needed
			if (m_components.num_allocated() + 1 > m_components.max_allocated())
				m_components.resize(m_components.max_allocated() + 32);

			// Allocate the component and call its constructor
			ResourceID id = m_components.allocate();
			T* component = m_components.get_resource_by_handle(id);
			::new(component)(T)(get_scene());

			// Call on_begin() on the system
			m_active_component = id;
			on_begin();

			return Handle<T>(id, &m_components);
		}

		/**
		 * @brief Remove a component from a system.
		 * @param Components ResourceID.
		 */
		void remove_component(ResourceID id)
		{
			dk_assert(m_components.is_allocated(id));

			// Call on_end() on the system
			m_active_component = id;
			on_end();

			// Deallocate the component
			m_components.deallocate(id);
		}

		/**
		 * @brief Get the active component.
		 */
		Handle<T> get_component()
		{
			return Handle<T>(m_active_component, &m_components);
		}

	private:

		/** Component resource allocater. */
		ResourceAllocator<T> m_components;

		/** Index of the active component. */
		ResourceID m_active_component = 0;
	};
}