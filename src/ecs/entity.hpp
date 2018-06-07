#pragma once

/**
 * @file entity.hpp
 * @brief ECS entity.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>

namespace dk
{
	class Scene;

	using EntityID = uint32_t;



	/**
	 * ECS entity.
	 */
	class Entity
	{
	public:

		/**
		 * Default constructor.
		 */
		Entity() : m_scene(nullptr), m_id(0) {}

		/**
		 * Constructor.
		 * @param Scene.
		 * @param Entity ID.
		 */
		Entity(Scene* scene, EntityID id);

		/**
		 * Constructor.
		 * @param Scene.
		 * @note Will create a new entity.
		 */
		Entity(Scene* scene);

		/**
		 * Destructor.
		 */
		~Entity() = default;

		/**
		 * Equivilence operator.
		 * @param Other entity.
		 * @return If the two handles are equal.
		 */
		inline bool operator==(const Entity& other) const
		{
			return m_id == other.m_id && m_scene == other.m_scene;
		}

		/**
		 * Unequivilence operator.
		 * @param Other entity.
		 * @return If the two handles are not equal.
		 */
		inline bool operator!=(const Entity& other) const
		{
			return m_id != other.m_id || m_scene != other.m_scene;
		}

		/**
		 * Get the scene the entity is in.
		 * @return Scene.
		 */
		inline Scene& get_scene() const
		{
			return *m_scene;
		}

		/**
		 * Get the entities ID.
		 * @param Entites ID.
		 */
		inline EntityID get_id() const
		{
			return m_id;
		}

		/**
		 * Add a component.
		 * @tparam Type of component.
		 * @param Entity the component will belong to.
		 * @return Component handle.
		 * @note Will not create a new component if one already exists for the given entity.
		 */
		template<class T>
		Handle<T> add_component();

		/**
		 * Get a component from an entity.
		 * @tparam Type of component.
		 * @param Entity the component belongs to.
		 * @return Component handle.
		 * @note Will return a Handle<T>() if the entity does not contain the component.
		 */
		template<class T>
		Handle<T> get_component();

		/**
		 * Remove a component from an entity.
		 * @tparam Type of component.
		 * @param entity the component belongs to.
		 */
		template<class T>
		void remove_component();

	private:

		/** Scene the entity exists in. */
		Scene* m_scene;

		/** Entities ID. */
		EntityID m_id;
	};
}