#pragma once

/**
 * @file system.hpp
 * @brief ECS system.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include <utilities\archive.hpp>
#include <engine\config.hpp>
#include "component.hpp"

/** 
 * Generate some basic body for the system 
 * @param System type.
 * @param Component type.
 * @param Should the system run in the editor?
 * @param Preallocated size.
 */
#define DK_SYSTEM_BODY(S, C, RIE, PA) \
S(dk::Scene* scene) : System<C>(scene, #S, RIE, PA) {} \
~S() = default; \

namespace dk
{
	/**
	 * ECS system base.
	 */
	class SystemBase
	{
	public:

		/**
		 * Constructor.
		 * @param Scene the system exists in.
		 * @param ID of the component the system works with.
		 * @param Name of the system.
		 * @param If the system runs in the editor.
		 * @note In order to not corrupt saved scenes, choose a scene name and don't change it.
		 */
		SystemBase(Scene* scene, type_id id, const std::string& name, bool run_in_editor = false);

		/**
		 * Destructor.
		 */
		virtual ~SystemBase();

		/**
		 * Get the scene.
		 * @return The scene.
		 */
		inline Scene& get_scene() const
		{
			return *m_scene;
		}

		/**
		 * Get the ID of the component the system works with.
		 * @return Component ID.
		 */
		inline type_id get_id() const
		{
			return m_component_id;
		}

		/**
		 * Get the name of the system.
		 * @return System name.
		 */
		inline std::string get_name() const
		{
			return m_name;
		}

		/**
		 * Get if the system runs in the editor.
		 * @return If the system runs in the editor.
		 */
		inline bool get_runs_in_editor() const
		{
			return m_run_in_editor;
		}

		/**
		 * Get a list of active component ID's.
		 * @return Active component ID's.
		 */
		virtual std::vector<ResourceID> get_active_components() const = 0;

		/**
		 * Get component allocator.
		 * @return Component allocator.
		 */
		virtual ResourceAllocatorBase* get_component_allocator() = 0;

		/**
		 * Remove a component from a system.
		 * @param Components entity.
		 */
		virtual void remove_component(Entity entity) = 0;

		/** 
		 * Remove every component from a system.
		 */
		virtual void remove_all_components() = 0;

		/**
		 * Called when a new entity is created.
		 * @param Entity created.
		 */
		virtual void on_new_entity(Entity entity);

		/**
		 * Called when a component is added to the system.
		 */
		virtual void on_begin();

		/**
		 * Called once per game tick.
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_tick(float delta_time);

		/**
		 * Called after on_tick().
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_late_tick(float delta_time);

		/**
		 * Called after on_late_tick() and before rendering.
		 * @param Time in seconds since the last frame.
		 */
		virtual void on_pre_render(float delta_time);

		/**
		 * Called when a component is removed from the system.
		 */
		virtual void on_end();

		/**
		 * Serialize the active component.
		 * @param Archiver.
		 */
		virtual void serialize(ReflectionContext& archive);

		/**
		 * Serialize the entire system.
		 * @param Archiver.
		 * @param Component archiver.
		 * @note The component archiver should use the archiver to read and write.
		 */
		virtual void serialize_system(Archive& archive, ReflectionContext& comp_archive);

		/**
		 * Serialize a component.
		 * @param Archiver.
		 * @param Entity.
		 * @return If the entity has the proper component.
		 */
		virtual bool serialize_by_entity(ReflectionContext& archive, Entity entity);

	private:

		/** Scene the system exists in. */
		Scene* m_scene;

		/** ID of the component the system works with. */
		type_id m_component_id;

		/** System name. */
		std::string m_name;

		/** If the system runs while in the editor. */
		bool m_run_in_editor;
	};

	/**
	 * Base class for user defined systems.
	 * @tparam Type of component the system works with.
	 */
	template<class T>
	class System : public SystemBase
	{
	public:

		/**
		 * Class used to iterate over components in a system.
		 */
		class Iterator
		{
		public:

			/**
			 * Default constructor.
			 */
			Iterator() = default;

			/**
			 * Constructor.
			 * @param System to iterate over.
			 * @param Starting position for the handle.
			 */
			Iterator(System* system, ResourceID pos) : m_system(system), m_handle(pos) {}

			/**
			 * Default destructor.
			 */
			~Iterator() = default;

			/**
			 * inequivalence operator.
			 * @param Left side.
			 * @param Right side.
			 * @return If the iterators are equal.
			 */
			bool operator!=(const Iterator& other) const
			{
				return m_handle != other.m_handle;
			}

			/**
			 * Increment operator.
			 * @return Self.
			 */
			Iterator& operator++()
			{
				++m_handle;

				while (m_handle != static_cast<uint32_t>(m_system->m_components.max_allocated()) && !m_system->m_components.is_allocated(m_handle))
					++m_handle;

				m_system->m_active_component = m_handle;
				return *this;
			}

			/**
			 * Dereference operator.
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
		 * Constructor.
		 * @param Scene the system exists in.
		 * @param Name of the system.
		 * @param If the systems runs in the editor.
		 * @param Number of components preallocated.
		 * @note In order to not corrupt saved scenes, choose a scene name and don't change it.
		 */
		System(Scene* scene, const std::string& name, bool run_in_editor = false, size_t pre_alloc = 32) : 
			SystemBase(scene, TypeID<T>::id(), name, run_in_editor), m_components(pre_alloc) {}

		/**
		 * Destructor.
		 */
		virtual ~System() {}

		/**
		 * Serialize the entire system.
		 * @param Archiver.
		 * @param Component archiver.
		 * @note The component archiver should use the archiver to read and write.
		 */
		void serialize_system(Archive& archive, ReflectionContext& comp_archive) override
		{
			// Save the old active component
			const ResourceID old_id = m_active_component;

			if (archive.is_writing())
			{
				// Write the minimum number of preallocated components needed
				uint32_t min_alloc = 0;
				for (uint32_t i = 0; i < m_components.max_allocated(); ++i)
					if (m_components.is_allocated(i))
						min_alloc = i;

				archive.write<uint32_t>(min_alloc + 1);

				// Write the number of components
				archive.write<uint32_t>(static_cast<uint32_t>(m_components.num_allocated()));

				// Write every component
				for(uint32_t i = 0; i < m_components.max_allocated(); ++i)
					if (m_components.is_allocated(i))
					{
						// Get the component
						T* component = m_components.get_resource_by_handle(i);

						// Write the component ID
						archive.write<uint32_t>(i);

						// Write the entity ID
						archive.write<EntityID>(component->get_entity().get_id());

						// Write the component
						m_active_component = static_cast<ResourceID>(i);
						serialize(comp_archive);
					}
			}
			else
			{
				// Get the number of preallocated components
				uint32_t pre_alloc = archive.read<uint32_t>();

				// Preallocate components
				m_components.resize(static_cast<size_t>(pre_alloc));

				// Get the number of components
				uint32_t comp_count = archive.read<uint32_t>();

				// Read every component
				for (uint32_t i = 0; i < comp_count; ++i)
				{
					// Read the component ID
					uint32_t comp_id = archive.read<uint32_t>();

					// Read the entity ID
					EntityID entity_id = archive.read<EntityID>();

					// Allocate the component and call its constructor
					T* component = nullptr;

					if (m_components.is_allocated(comp_id))
						component = m_components.get_resource_by_handle(comp_id);
					else
					{
						m_components.allocate_by_id(comp_id);
						component = m_components.get_resource_by_handle(comp_id);
						::new(component)(T)(Handle<T>(comp_id, &m_components), Entity(&get_scene(), entity_id));
					}

					// Deserialize the component
					m_active_component = comp_id;
					serialize(comp_archive);
				}
			}

			// Call on_begin() for every component at once
			for(ResourceID i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
					m_active_component = i;

#if DK_EDITOR
					if (get_runs_in_editor())
#endif
					{
						on_begin();
					}
				}

			// Put back old active component
			m_active_component = old_id;
		}

		/**
		 * Serialize a component.
		 * @param Archiver.
		 * @param Entity.
		 * @return If the entity has the proper component.
		 */
		bool serialize_by_entity(ReflectionContext& archive, Entity entity) override final
		{
			dk_assert(&entity.get_scene() == &get_scene());
			const Handle<T> comp = find_component_by_entity(entity);
			if (comp == Handle<T>())
				return false;

			// Serialize the component
			const ResourceID old_id = m_active_component;
			m_active_component = comp.id;
			serialize(archive);
			m_active_component = old_id;
			return true;
		}

		/**
		 * Get a list of active component ID's.
		 * @return Active component ID's.
		 */
		std::vector<ResourceID> get_active_components() const override final
		{
			std::vector<ResourceID> ids(m_components.num_allocated());
			size_t index = 0;
			for(ResourceID i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
					ids[index] = static_cast<ResourceID>(i);
					++index;
				}
			return ids;
		}

		/**
		 * Get component allocator.
		 * @return Component allocator.
		 */
		ResourceAllocatorBase* get_component_allocator() override
		{
			return &m_components;
		}

		/**
		 * Add a component to the system.
		 * @param Entity the component belongs to.
		 * @return Components handle.
		 */
		Handle<T> add_component(Entity entity)
		{
			// Resize allocator if needed
			if (m_components.num_allocated() + 1 > m_components.max_allocated())
				m_components.resize(m_components.max_allocated() + 32);

			// Allocate the component and call its constructor
			const ResourceID id = m_components.allocate();
			T* component = m_components.get_resource_by_handle(id);
			::new(component)(T)(Handle<T>(id, &m_components), entity);

			// Call on_begin() on the system
#if DK_EDITOR
			if (get_runs_in_editor())
#endif
			{
				m_active_component = id;
				on_begin();
			}

			return Handle<T>(id, &m_components);
		}

		/**
		 * Find a component via it's entity
		 * @param Entity of the component.
		 * @return The component.
		 * @note Will return a Handle<T>() if the component is not found.
		 */
		Handle<T> find_component_by_entity(Entity entity)
		{
			for (ResourceID i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
					const T* component = m_components.get_resource_by_handle(i);

					if (component->get_entity() == entity)
						return Handle<T>(i, &m_components);
				}

			return Handle<T>(0, nullptr);
		}

		/**
		 * Remove a component from a system.
		 * @param Components ResourceID.
		 */
		void remove_component(Entity entity) override
		{
			const Handle<T> component = find_component_by_entity(entity);
			if (!component.allocator) return;

			// Call on_end() on the system
#if DK_EDITOR
			if (get_runs_in_editor())
#endif
			{
				m_active_component = component.id;
				on_end();
			}

			// Deallocate the component
			m_components.deallocate(component.id);
		}

		/**
		 * Remove every component from a system.
		 */
		void remove_all_components() override
		{
			for (ResourceID i = 0; i < m_components.max_allocated(); ++i)
				if (m_components.is_allocated(i))
				{
#if DK_EDITOR
					if (get_runs_in_editor())
#endif
					{
						m_active_component = static_cast<ResourceID>(i);
						on_end();
					}
					m_components.deallocate(i);
				}
		}

		/**
		 * Get the active component.
		 */
		Handle<T> get_component()
		{
			return Handle<T>(m_active_component, &m_components);
		}

		/**
		 * Get iterator at the beginning of the component list.
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
		 * Get iterator at the end of the component list.
		 * @return Iterator at the end of the component list.
		 */
		Iterator end()
		{
			return Iterator(this, static_cast<ResourceID>(m_components.max_allocated()));
		}

	private:

		/** Component resource allocater. */
		ResourceAllocator<T> m_components;

		/** Index of the active component. */
		ResourceID m_active_component = 0;
	};
}