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

		// File
		if (ImGui::BeginMenu("File"))
		{
			// Quit
			ImGui::MenuItem("Quit", nullptr, &m_closing);

			ImGui::EndMenu();
		}

		// End menu bar
		ImGui::EndMainMenuBar();
	}
}