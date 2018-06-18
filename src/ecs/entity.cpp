/**
 * @file entity.cpp
 * @brief Entity source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "scene.hpp"
#include "entity.hpp"

namespace dk
{
	Entity::Entity() : m_scene(nullptr), m_id(0) {}

	Entity::Entity(Scene* scene, entity_id id) : m_scene(scene), m_id(id) {}
}