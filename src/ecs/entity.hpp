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

	using EntityID = size_t;



	/**
	 * @brief ECS entity.
	 */
	class Entity
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Entity() : m_scene(nullptr), m_id(0) {}

		/**
		 * @brief Constructor.
		 * @param Scene.
		 * @param Entity ID.
		 */
		Entity(Scene* scene, EntityID id);

		/**
		 * @brief Constructor.
		 * @param Scene.
		 * @note Will create a new entity.
		 */
		Entity(Scene* scene);

		/**
		 * @brief Destructor.
		 */
		~Entity() = default;

		/**
		 * @brief Equivilence operator.
		 * @param Other entity.
		 * @return If the two handles are equal.
		 */
		bool operator==(const Entity& other) const
		{
			return m_id == other.m_id && m_scene == other.m_scene;
		}

		/**
		 * @brief Unequivilence operator.
		 * @param Other entity.
		 * @return If the two handles are not equal.
		 */
		bool operator!=(const Entity& other) const
		{
			return m_id != other.m_id || m_scene != other.m_scene;
		}

		/**
		 * @brief Get the scene the entity is in.
		 * @return Scene.
		 */
		Scene& get_scene() const
		{
			return *m_scene;
		}

		/**
		 * @brief Get the entities ID.
		 * @param Entites ID.
		 */
		EntityID get_id() const
		{
			return m_id;
		}

		/**
		 * @brief Add a component.
		 * @tparam Type of component.
		 * @param Entity the component will belong to.
		 * @return Component handle.
		 * @note Will not create a new component if one already exists for the given entity.
		 */
		template<class T>
		Handle<T> add_component();

		/**
		 * @brief Get a component from an entity.
		 * @tparam Type of component.
		 * @param Entity the component belongs to.
		 * @return Component handle.
		 * @note Will return a Handle<T>() if the entity does not contain the component.
		 */
		template<class T>
		Handle<T> get_component();

		/**
		 * @brief Remove a component from an entity.
		 * @tparam Type of component.
		 * @param entity the component belongs to.
		 */
		template<class T>
		void remove_component();

	private:

		/** Scene the entity exists in. */
		Scene* m_scene = nullptr;

		/** Entities ID. */
		EntityID m_id = 0;
	};
}