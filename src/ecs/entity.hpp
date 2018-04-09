#pragma once

/**
 * @file entity.hpp
 * @brief ECS entity.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */

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
		 * @brief Get the scene the entity is in.
		 * @return Scene.
		 */
		Scene* get_scene() const
		{
			return m_scene;
		}

		/**
		 * @brief Get the entities ID.
		 * @param Entites ID.
		 */
		EntityID get_id() const
		{
			return m_id;
		}

	private:

		/** Scene the entity exists in. */
		Scene* m_scene = nullptr;

		/** Entities ID. */
		EntityID m_id = 0;
	};
}