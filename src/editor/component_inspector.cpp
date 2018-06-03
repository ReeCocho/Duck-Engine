#pragma once

/**
 * @file component_inspector.cpp
 * @brief Component inspector source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "component_inspector.hpp"

namespace dk
{
	ComponentInspector::ComponentInspector(Scene* scene, ResourceManager* resource_manager) :
		m_scene(scene),
		m_resource_manager(resource_manager),
		m_mode(Mode::Name),
		m_name(""),
		m_fields({})
	{

	}

	ComponentInspector::~ComponentInspector()
	{
		for (auto field : m_fields)
			delete field;
	}

	void ComponentInspector::set_name(const std::string& name)
	{
		dk_assert(m_mode == Mode::Name);
		m_name = name;
		m_mode = Mode::Field;
	}

	bool ComponentInspector::field_exists(const std::string& name)
	{
		for (auto field : m_fields)
			if (field->name == name)
				return true;
		return false;
	}
}