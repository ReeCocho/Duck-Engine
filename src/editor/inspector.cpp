/**
 * @file inspector.cpp
 * @brief Inspector source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "inspector.hpp"

namespace dk
{
	Inspector::Inspector(Graphics* graphics, Scene* scene) :
		m_graphics(graphics),
		m_scene(scene)
	{

	}

	void Inspector::draw()
	{
		// Get window dimensions
		float height = static_cast<float>(m_graphics->get_height());
		float width = static_cast<float>(m_graphics->get_width());

		ImGui::Text("Inspector");

		// Begin window
		if (m_inspected_entity != Entity(nullptr, 0))
		{
			
		}
	}
}