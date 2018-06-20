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
		m_transform_system = static_cast<System<Transform>*>(m_scene->get_system_by_id(TypeID<Transform>::id()));
		dk_assert(m_transform_system);
	}

	void EditorHierarchy::draw()
	{
		// Get window dimensions
		float height = static_cast<float>(m_graphics->get_height());
		float width = static_cast<float>(m_graphics->get_width());

		// Context menu
		if (ImGui::BeginPopupContextWindow("Hierarchy Context Menu"))
		{
			if (ImGui::MenuItem("New entity"))
				m_scene->create_entity();

			ImGui::EndPopup();
		}

		// Loop over every transform and find which ones have no parent (roots)
		for (Handle<Transform> transform : *m_transform_system)
		{
			if (transform->get_parent() == Handle<Transform>())
				draw_transform_tree(transform);
		}
	}

	void EditorHierarchy::draw_transform_tree(Handle<Transform> transform)
	{
		// Node flags
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		// Make it a leaf if it has no children
		if (transform->child_count() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		// Create new tree element (Use transform ID since it's unique)
		std::string label = "Entity " + std::to_string(transform.id);
		bool node_open = ImGui::TreeNodeEx((const void*)(uintptr_t)transform.id, flags, label.data());
		
		// Context menu
		ImGui::PushID((const void*)(uintptr_t)transform.id);
		if (ImGui::BeginPopupContextItem("Entity Context Menu"))
		{
			// Destroy the entity
			if (ImGui::MenuItem("Destroy entity"))
			{
				// Reset the inspected entity if we just destroyed it
				if (transform->get_entity() == m_inspector->get_inspected_entity())
					m_inspector->inspect_entity(Entity());

				// Destroy the entity
				m_scene->destroy_entity(transform->get_entity());

				ImGui::EndPopup();
				ImGui::PopID();

				if(node_open) ImGui::TreePop();
				return;
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();

		// Inspect the entity if the node is clicked
		if(ImGui::IsItemClicked())
			m_inspector->inspect_entity(transform->get_entity());

		// Drag and drop the entity
		if (ImGui::BeginDragDropSource())
		{
			ImGui::Text(label.data());
			ImGui::SetDragDropPayload("resource_id", &transform.id, sizeof(resource_id));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource_id"))
			{
				// Get transform
				resource_id transform_id = 0;
				std::memcpy(&transform_id, payload->Data, payload->DataSize);

				// Only allow for drag and drop on transforms that aren't ourselves
				if (transform_id != transform.id)
					transform->add_child(dk::Handle<Transform>(transform_id, transform.allocator));
			}
			ImGui::EndDragDropTarget();
		}

		// Draw children if the node is expanded
		if(node_open)
		{
			// Draw every childs tree
			for (size_t i = 0; i < transform->child_count(); ++i)
				draw_transform_tree(transform->get_child(i));

			// Pop tree off the stack
			ImGui::TreePop();
		}
	}
}