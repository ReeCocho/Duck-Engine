/**
 * @file scene.cpp
 * @brief ECS scene source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "scene.hpp"

namespace dk
{
	Scene::Scene()
	{

	}

	void Scene::shutdown()
	{
		m_systems.clear();
	}
}