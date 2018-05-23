/**
 * @file system.cpp
 * @brief ECS system source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "system.hpp"

namespace dk
{
	SystemBase::SystemBase(Scene* scene, size_t id, bool rie) : 
		m_scene(scene), 
		m_component_id(id),
		m_run_in_editor(rie)
	{

	}

	SystemBase::~SystemBase()
	{

	}

	void SystemBase::on_begin()
	{

	}

	void SystemBase::on_tick(float delta_time)
	{

	}

	void SystemBase::on_late_tick(float delta_time)
	{

	}

	void SystemBase::on_pre_render(float delta_time)
	{

	}

	void SystemBase::on_end()
	{
		
	}
}