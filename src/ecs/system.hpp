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
		Scene& get_scene() const
		{
			return *m_scene;
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
		 * @brief Remove a component from a system.
		 * @param Components entity.
		 */
		virtual void remove_component(Entity entity) = 0;

		/** 
		 * @brief Remove every component from a system.
		 */
		virtual void remove_all_components() = 0;

		/**
		 * @brief Called when a component is added to the system.
		 */
		virtual void on_begin();

		/**
		 * @brief Called once per game tick.
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_tick(float delta_time);

		/**
		 * @brief Called after on_tick().
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_late_tick(float delta_time);

		/**
		 * @brief Called after on_late_tick() and before rendering.
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_pre_render(float delta_time);

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
		 * @class Iterator
		 * @brief Class used to iterate over components in a system.
		 */
		class Iterator
		{
		public:

			/**
			 * @brief Default constructor.
			 */
			Iterator() = default;

			/**
			 * @brief Constructor.
			 * @param System to iterate over.
			 * @param Starting position for the handle.
			 */
			Iterator(System* system, size_t pos) : m_system(system), m_handle(pos) {}

			/**
			 * @brief Default destructor.
			 */
			~Iterator() = default;

			/**
			 * @brief inequivalence operator.
			 * @param Left side.
			 * @param Right side.
			 * @return If the iterators are equal.
			 */
			bool operator!=(const Iterator& other) const
			{
				return m_handle != other.m_handle;
			}

			/**
			 * @brief Increment operator.
			 * @return Self.
			 */
			Iterator& operator++()
			{
				++m_handle;

				while (m_handle != m_system->m_components.max_allocated() && !m_system->m_components.is_allocated(m_handle))
					++m_handle;

				m_system->m_active_component = m_handle;
				return *this;
			}

			/**
			 * @brief Indirection operator.
			 * @return Active component.
			 */
			Handle<T> operator*()
			{
				return Handle<T>(m_handle, &m_system->m_components);
			}

		private:

			/** System to iterate over. */
			System* m_system = nullptr;

			/** Handle. */
			ResourceID m_handle = 0;
		};



		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 * @param Number of components preallocated.
		 */
		System(Scene* scene, size_t pre_alloc = 32) : SystemBase(scene, TypeID<T>::id()), m_components(pre_alloc) {}

		/**
		 * @brief Destructor.
		 */
		virtual ~System() {}

		/**
		 * @brief Add a component to the system.
		 * @param Entity the component belongs to.
		 * @return Components handle.
		 */
		Handle<T> add_component(Entity entity)
		{
			// Resize allocator if needed
			if (m_components.num_allocated() + 1 > m_components.max_allocated())
				m_components.resize(m_components.max_allocated() + 32);

			// Allocate the component and call its constructor
			ResourceID id = m_components.allocate();
			T* component = m_components.get_resource_by_handle(id);
			::new(component)(T)(&get_scene(), entity);

			// Call on_begin() on the system
			m_active_component = id;
			on_begin();

			return Handle<T>(id, &m_components);
		}

		/**
		 * @brief Find a component via it's entity
		 * @param Entity of the component.
		 * @return The component.
		 * @note Will return a Handle<T>() if the component is not found.
		 */
		Handle<T> find_component_by_entity(Entity entity)
		{
			for (size_t i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
					T* component = m_components.get_resource_by_handle(i);

					if (component->get_entity() == entity)
						return Handle<T>(i, &m_components);
				}

			return Handle<T>(0, nullptr);
		}

		/**
		 * @brief Remove a component from a system.
		 * @param Components ResourceID.
		 */
		void remove_component(Entity entity) override
		{
			Handle<T> component = find_component_by_entity(entity);
			if (!component.allocator) return;

			// Call on_end() on the system
			m_active_component = component.id;
			on_end();

			// Deallocate the component
			m_components.deallocate(component.id);
		}

		/**
		 * @brief Remove every component from a system.
		 */
		void remove_all_components() override
		{
			for (size_t i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
					m_active_component = static_cast<ResourceID>(i);
					on_end();
					m_components.deallocate(i);
				}
		}

		/**
		 * @brief Get the active component.
		 */
		Handle<T> get_component()
		{
			return Handle<T>(m_active_component, &m_components);
		}

		/**
		 * @brief Get iterator at the beginning of the component list.
		 * @return Iterator at the beginning of the component list.
		 */
		Iterator begin()
		{
			ResourceID index = 0;
			for (index; index <= m_components.max_allocated(); ++index)
			{
				if (index == m_components.max_allocated() || m_components.is_allocated(index))
					break;
			}

			return Iterator(this, index);
		}

		/**
		 * @brief Get iterator at the end of the component list.
		 * @return Iterator at the end of the component list.
		 */
		Iterator end()
		{
			return Iterator(this, m_components.max_allocated());
		}

	private:

		/** Component resource allocater. */
		ResourceAllocator<T> m_components;

		/** Index of the active component. */
		ResourceID m_active_component = 0;
	};
}