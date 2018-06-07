/**
 * @file system.cpp
 * @brief ECS system source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "system.hpp"

namespace dk
{
	SystemBase::SystemBase(Scene* scene, type_id id, const std::string& name, bool rie) :
		m_scene(scene), 
		m_component_id(id),
		m_name(name),
		m_run_in_editor(rie)
	{

	}

	SystemBase::~SystemBase()
	{

	}

	void SystemBase::on_new_entity(Entity entity)
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

	void SystemBase::serialize(ReflectionContext& archive)
	{

	}

	bool SystemBase::serialize_by_entity(ReflectionContext& archive, Entity entity)
	{
		return false;
	}

	void SystemBase::serialize_system(Archive& archive, ReflectionContext& comp_archive)
	{

	}
}