#pragma once

/** Includes. */
#include <algorithm>
#include <utilities\reflection.hpp>
#include <utilities\resource_allocator.hpp>
#include "entity.hpp"

namespace dk
{
	/** Forward declare component. */
	template<class T>
	class Component;

	/** Forward declare the scene. */
	class Scene;

	/**
	 * System interface.
	 */
	class ISystem
	{
	public:

		/**
		 * Constructor.
		 * @param Scene the system exists in.
		 * @param Name of the system.
		 * @param Type of component the system works with.
		 * @param If the system runs in the editor.
		 */
		ISystem(Scene* scene, const std::string& name, type_id component_type, bool runs_in_editor);

		/**
		 * Destructor.
		 */
		virtual ~ISystem();

		/**
		 * Get the type of component the system acts upon.
		 * @return Component type.
		 */
		inline type_id get_component_type() const;

		/**
		 * Get if the system runs in the editor.
		 * @return If the system runs in the editor.
		 */
		inline bool runs_in_editor() const;

		/**
		 * Get the scene the system exists in.
		 * @return Scene.
		 */
		inline Scene& get_scene() const;

		/**
		 * Get the name of the system.
		 * @return System name.
		 */
		inline std::string get_name() const;

		/**
		 * Get the IDs of every active component in the system.
		 * @return Active component IDs.
		 */
		virtual std::vector<resource_id> get_active_components() const = 0;

		/**
		 * Add a component to an entity.
		 * @param Entity to add the component to.
		 * @note If the entity already has the component, nothing will happen.
		 */
		virtual void add_component(const Entity& e) = 0;

		/**
		 * Check if an entity has a component.
		 * @param Entity.
		 * @return If the entity has the component.
		 */
		virtual bool has_component(const Entity& e) = 0;

		/**
		 * Remove a component from an entity.
		 * @param Entity to remove the component from.
		 * @note If the entity does not have the component nothing will happen.
		 */
		virtual void remove_component(const Entity& e) = 0;

		/**
		 * Load a component.
		 * @param Components ID.
		 * @param Entity the component should be attached to.
		 * @param Function used to load the component.
		 */
		virtual void load_component(resource_id id, const Entity& e, std::function<void(ReflectionContext&)>& load) = 0;

		/**
		 * Called when a new entity is added to the scene.
		 * @param New entity.
		 */
		virtual void on_new_entity(const Entity& e);

		/**
		 * Called when a component is added to the system.
		 */
		virtual void on_begin();

		/**
		 * Called once per frame.
		 * @param Time since the last frame.
		 */
		virtual void on_tick(float dt);

		/**
		 * Called once per frame after on_tick().
		 * @param Time since the last frame.
		 */
		virtual void on_late_tick(float dt);

		/**
		 * Called before rendering but after on_late_tick().
		 * @param Time since the last frame.
		 */
		virtual void on_pre_render(float dt);

		/**
		 * Called when a component is removed from the system.
		 */
		virtual void on_end();

		/**
		 * Serialize the active component.
		 * @param Reflection context.
		 */
		virtual void serialize(ReflectionContext& r) = 0;

		/**
		 * Inspect the active component.
		 * @param Reflection context.
		 */
		virtual void inspect(ReflectionContext& r) = 0;

		/**
		 * Get a components entity by its resource ID.
		 * @param Component resource ID.
		 * @return Components entity.
		 */
		virtual Entity get_entity_by_component_by_id(resource_id comp_id) = 0;

		/**
		 * Get a component ID from it's entity.
		 * @param Components entity.
		 * @return Component ID.
		 * @note Will throw a runtime error if the component cannot be found.
		 */
		virtual resource_id get_component_id_by_entity(const Entity& e) = 0;

		/**
		 * Get the systems component allocator.
		 * @return Component allocator.
		 */
		virtual ResourceAllocatorBase& get_component_allocator() = 0;

		/**
		 * Set the active component.
		 * @param Active component ID.
		 * @note This should not be used when iterating over components.
		 */
		inline void set_active_component(resource_id component_id);

	private:

		/** Scene the system exists in. */
		Scene * m_scene;

		/** Name of the system. */
		std::string m_name;

		/** Type ID of the component that the system acts upon. */
		type_id m_component_type;

	protected:

		/** Does the system run in the editor? */
		bool m_runs_in_editor;

		/** Active component in the system. */
		resource_id m_active_component;
	};

	/**
	 * Macro to create a constructor and destructor for convenience.
	 * @param System type.
	 * @param Type of component the system operates on.
	 * @param If the system should run in the editor or not.
	 */
	#define DK_SYSTEM_BODY(S, C, RIE) \
	S(dk::Scene* scene) : dk::System<C>(scene, #S, RIE) {} \
	~S() = default;

	 /** A group of functionality which acts upon a type of component. */
	template<class C>
	class System : public ISystem
	{
		static_assert(std::is_convertible<C, Component<C>>::value, "C must derive from Component<C>.");

	public:

		/**
		 * System iterator.
		 */
		class iterator
		{
		public:

			/**
			 * Constructor.
			 * @param System we are iterating over.
			 * @param Starting component id.
			 */
			iterator(System* system, resource_id start_component);

			/**
			 * Post incremenet.
			 * @return Iterator one step forward.
			 */
			inline iterator& operator++();

			/**
			 * Unequivilence operator.
			 * @param Other iterator.
			 * @return If the two iterators are not equalivilent.
			 */
			inline bool operator!=(const iterator& other) const;

			/**
			 * Dereference operator.
			 * @return Active component of the iterated system.
			 */
			inline Handle<C> operator*() const;

		private:

			/** System we are iterating over. */
			System * m_system;

			/** Iterator active component. */
			resource_id m_component;
		};

		/**
		 * Constructor.
		 * @param Scene the system resides in.
		 * @param Name of the system.
		 * @param Does the system run in the editor?
		 */
		System(Scene* scene, const std::string& name, bool runs_in_editor);

		/**
		 * Destructor.
		 */
		virtual ~System();

		/**
		 * Get the number of components in the system.
		 * @return Number of components in the system.
		 */
		inline size_t get_component_count() const;

		/**
		 * Get the active component.
		 * @return Active component.
		 */
		inline Handle<C> get_active_component();

		/**
		 * Get a component that belongs to an entity.
		 * @param Entity to check.
		 * @return Component that belongs to the entity.
		 * @note If the entity does not have the component a null handle will be returned.
		 */
		Handle<C> get_component(const Entity& e);

		/**
		 * Add a component to an entity.
		 * @param Entity to add the component to.
		 * @note If the entity already has the component, nothing will happen.
		 */
		void add_component(const Entity& e) override;

		/**
		 * Check if an entity has a component.
		 * @param Entity.
		 * @return If the entity has the component.
		 */
		bool has_component(const Entity& e) override;

		/**
		 * Remove a component from an entity.
		 * @param Entity to remove the component from.
		 * @note If the entity does not have the component nothing will happen.
		 */
		void remove_component(const Entity& e) override;

		/**
		 * Get the IDs of every active component in the system.
		 * @return Active component IDs.
		 */
		std::vector<resource_id> get_active_components() const override;

		/**
		 * Get a components entity by its resource ID.
		 * @param Component resource ID.
		 * @return Components entity.
		 */
		Entity get_entity_by_component_by_id(resource_id comp_id) override;

		/**
		 * Get a component ID from it's entity.
		 * @param Components entity.
		 * @return Component ID.
		 * @note Will throw a runtime error if the component cannot be found.
		 */
		resource_id get_component_id_by_entity(const Entity& e) override;

		/**
		 * Get the systems component allocator.
		 * @return Component allocator.
		 */
		ResourceAllocatorBase& get_component_allocator() override;

		/**
		 * Load a component.
		 * @param Components ID.
		 * @param Entity the component should be attached to.
		 * @param Function used to load the component.
		 */
		void load_component(resource_id id, const Entity& e, std::function<void(ReflectionContext&)>& load) override;

		/**
		 * Begin iterating over the systems components.
		 * @return System iterator.
		 */
		inline iterator begin();

		/**
		 * Iterator at the end of the component list.
		 * @return System iterator.
		 */
		inline iterator end();

	private:

		/** Component allocator. */
		ResourceAllocator<C> m_allocator;
	};
}

#include "system.imp.hpp"