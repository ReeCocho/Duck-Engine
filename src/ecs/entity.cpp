/**
 * @file entity.cpp
 * @brief ECS entity source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "scene.hpp"
#include "entity.hpp"

namespace dk
{
	Entity::Entity(Scene* scene, EntityID id) : m_scene(scene), m_id(id) {}

	Entity::Entity(Scene* scene) : m_scene(scene), m_id(m_scene->create_entity())
	{
		
	}
}