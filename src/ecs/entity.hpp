#pragma once

/**
 * @file entity.hpp
 * @brief A handle associated with a set of components.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>

namespace dk
{
	/** Forward declare the scene. */
	class Scene;

	/** Unique entity ID. */
	using entity_id = uint32_t;

	/**
	 * A handle associated with a set of components.
	 */
	class Entity
	{
	public:

		/**
		 * Default constructor.
		 * @note Results in an invalid entity.
		 */
		Entity();

		/**
		 * Constructor.
		 * @param Scene the entity exists in.
		 * @param Entities ID.
		 */
		Entity(Scene* scene, entity_id id);

		/**
		 * Destructor.
		 */
		~Entity() = default;

		/**
		 * Check if two entities are equal.
		 * @param Other entity.
		 * @return If the two entities are equal.
		 */
		inline bool operator==(const Entity& other) const;

		/**
		 * Check if two entities are not equal.
		 * @param Other entity.
		 * @return If the two entities are not equal.
		 */
		inline bool operator!=(const Entity& other) const;

		/**
		 * Get the scene the entity exists in.
		 * @return Scene.
		 */
		inline Scene& get_scene() const;

		/**
		 * Get the ID of the entity.
		 * @return Entity ID.
		 */
		inline entity_id get_id() const;

		/**
		 * Determine if this entity is a valid entity handle.
		 * @return If this entity handle is valid.
		 */
		inline bool is_valid() const;

		/**
		 * Get a component that belongs to this entity.
		 * @tparam Type of component to retrieve.
		 * @return Component which belongs to this entity.
		 * @note Type C must be derived from Component<C>.
		 * @note If the component could not be found a null handle will be returned.
		 */
		template<class C>
		inline Handle<C> get_component() const;

		/**
		 * Add a component to the entity.
		 * @tparam Type of component to add.
		 * @return Component that was added.
		 * @note Type C must be derived from Component<C>.
		 * @note If the entity already has a component of that type, the already existing component will be returned.
		 */
		template<class C>
		inline Handle<C> add_component() const;

		/**
		 * Remove a component from the entity.
		 * @tparam Type of component to remove.
		 * @note Type C must be derived from Component<C>.
		 * @note If the entity does not have the component nothing will happen.
		 */
		template<class C>
		inline void remove_component() const;

	private:

		/** Entities ID. */
		entity_id m_id;

		/** Scene the entity exists in. */
		Scene* m_scene;
	};
}

#include "entity.imp.hpp"