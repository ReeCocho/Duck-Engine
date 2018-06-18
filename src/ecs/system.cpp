/**
 * @file system.cpp
 * @brief System source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\debugging.hpp>
#include "scene.hpp"

namespace dk
{
	ISystem::ISystem(Scene* scene, const std::string& name, type_id component_type, bool runs_in_editor) :
		m_scene(scene),
		m_name(name),
		m_component_type(component_type),
		m_runs_in_editor(runs_in_editor),
		m_active_component(0)
	{ dk_assert(m_scene); }

	ISystem::~ISystem() {}

	void ISystem::on_new_entity(const Entity& e) {}

	void ISystem::on_begin() {}

	void ISystem::on_tick(float dt) {}

	void ISystem::on_late_tick(float dt) {}

	void ISystem::on_pre_render(float dt) {}

	void ISystem::on_end() {}
}