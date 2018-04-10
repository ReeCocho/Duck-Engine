/**
 * @file system.cpp
 * @brief ECS system source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "system.hpp"

namespace dk
{
	SystemBase::SystemBase(Scene* scene, size_t id) : m_scene(scene), m_component_id(id)
	{

	}

	SystemBase::~SystemBase()
	{

	}

	void SystemBase::on_begin()
	{

	}

	void SystemBase::on_tick()
	{

	}

	void SystemBase::on_late_tick()
	{

	}

	void SystemBase::on_pre_render()
	{

	}

	void SystemBase::on_end()
	{

	}
}