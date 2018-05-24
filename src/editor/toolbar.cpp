/**
 * @file toolbar.cpp
 * @brief Tool bar source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "toolbar.hpp"

namespace dk
{
	Toolbar::Toolbar(Graphics* graphics, Scene* scene) :
		m_graphics(graphics),
		m_scene(scene)
	{

	}

	void Toolbar::draw()
	{
		// Start menu bar
		ImGui::BeginMainMenuBar();

		// End menu bar
		ImGui::EndMainMenuBar();
	}
}