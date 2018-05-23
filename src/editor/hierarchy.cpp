/**
 * @file hierachy.cpp
 * @brief Hierarchy source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "hierarchy.hpp"

namespace dk
{
	EditorHierarchy::EditorHierarchy(Graphics* graphics, Scene* scene, Inspector* inspector) : 
		m_scene(scene),
		m_graphics(graphics),
		m_inspector(inspector)
	{
		m_transform_system = m_scene->get_system<Transform>();
		dk_assert(m_transform_system);
	}

	void EditorHierarchy::draw()
	{
		// Get window dimensions
		float height = static_cast<float>(m_graphics->get_height());
		float width = static_cast<float>(m_graphics->get_width());

		// Begin window
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(width / 4.0f, height));
		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		// Loop over every transform and find which ones have no parent (roots)
		for (Handle<Transform> transform : *m_transform_system)
		{
			if (transform->get_parent() == Handle<Transform>())
				draw_transform_tree(transform);
		}

		// End window
		ImGui::End();
	}

	void EditorHierarchy::draw_transform_tree(Handle<Transform> transform)
	{
		// Create new tree element (Use transform ID since it's unique)
		ImGui::TreePush((void*)transform.id);

		// Inspect the entity
		if (ImGui::Button("Entity"))
		{
			m_inspector->inspect_entity(transform->get_entity());
		}

		// Draw every childs tree
		for (size_t i = 0; i < transform->child_count(); ++i)
			draw_transform_tree(transform->get_child(i));

		// Pop tree off the stack
		ImGui::TreePop();
	}
}