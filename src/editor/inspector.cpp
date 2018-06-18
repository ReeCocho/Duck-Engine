/**
 * @file inspector.cpp
 * @brief Inspector source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "inspector.hpp"

namespace dk
{
	Inspector::Inspector(Graphics* graphics, Scene* scene, ResourceManager* resource_manager) :
		m_graphics(graphics),
		m_scene(scene),
		m_resource_manager(resource_manager)
	{

	}

	void Inspector::inspect_entity(Entity entity)
	{
		m_component_inspectors.clear();
		m_inspected_entity = entity;

		// Only continue if the entity is valid
		if (!entity.is_valid())
			return;

		// Find component fields
		for (size_t i = 0; i < m_scene->get_system_count(); ++i)
		{
			// System for convenience
			ISystem& system = m_scene->get_system_by_index(i);

			// Check if the entity has the component
			if (system.has_component(entity))
			{
				// Inspect the component
				auto r = std::make_unique<ReflectionContext>();
				system.set_active_component(system.get_component_id_by_entity(m_inspected_entity));
				system.inspect(*r);

				// Add the reflection context to the list
				m_component_inspectors.push_back(std::make_tuple(system.get_component_type(), std::move(r)));
			}
		}
	}

	void Inspector::draw()
	{
		// Get window dimensions
		float height = static_cast<float>(m_graphics->get_height());
		float width = static_cast<float>(m_graphics->get_width());

		// Begin window
		if (m_inspected_entity != Entity(nullptr, 0))
		{
			// Loop over every system archive
			for (const auto& archive : m_component_inspectors)
			{
				auto comp_id = std::get<0>(archive);
				auto& reflection = std::get<1>(archive);

				// Show component fields
				bool fields_shown = ImGui::CollapsingHeader(reflection->get_name().data());

				// Context menu
				ImGui::PushID(reflection->get_name().data());
				if (ImGui::BeginPopupContextItem("Component Context Menu"))
				{
					const std::string label = "Destroy " + reflection->get_name();

					// Destroy the entity
					if (ImGui::Button(label.data()))
					{
						// Make it so you can't destroy transforms since they are required
						if (comp_id != TypeID<Transform>::id())
						{
							m_scene->get_system_by_id(comp_id)->remove_component(m_inspected_entity);
							inspect_entity(m_inspected_entity);
							ImGui::EndPopup();
							ImGui::PopID();
							break;
						}
					}
					ImGui::EndPopup();
				}
				ImGui::PopID();

				if(fields_shown)
				{
					const auto& fields = reflection->get_fields();

					// Loop over every field
					for (const auto& field : fields)
						draw_field(field.get());
				}
			}

			// Add component menu
			ImGui::Separator();
			if(ImGui::CollapsingHeader("Add Components"))
				if (ImGui::ListBoxHeader("##", ImVec2(-1, 0)))
				{
					for (size_t i = 0; i < m_scene->get_system_count(); ++i)
					{
						ISystem& system = m_scene->get_system_by_index(i);
						const std::string label = "Add " + system.get_name() + " component";
						ImGui::PushItemWidth(-1.0f);
						
						// Add the component
						if (ImGui::Button(label.data(), ImVec2(-1, 0)))
						{
							system.add_component(m_inspected_entity);

							// Reinspect the entity to update the inspector view
							inspect_entity(m_inspected_entity);
						}
					}
					ImGui::ListBoxFooter();
				}
		}
	}

	void Inspector::draw_field(ReflectionContext::Field* field)
	{
		bool run_callback = false;

		// 32 bit integer
		if (field->type_id == TypeID<int32_t>::id())
			run_callback = ImGui::InputScalar(field->name.data(), ImGuiDataType_S32, static_cast<int32_t*>(field->data));
		// 64 bit integer
		else if (field->type_id == TypeID<int64_t>::id())
			run_callback = ImGui::InputScalar(field->name.data(), ImGuiDataType_S64, static_cast<int64_t*>(field->data));
		// 32 bit unsigned integer
		else if (field->type_id == TypeID<uint32_t>::id())
			run_callback = ImGui::InputScalar(field->name.data(), ImGuiDataType_U32, static_cast<uint32_t*>(field->data));
		// 64 bit unsigned integer
		else if (field->type_id == TypeID<uint64_t>::id())
			run_callback = ImGui::InputScalar(field->name.data(), ImGuiDataType_U64, static_cast<uint64_t*>(field->data));
		// Float
		else if (field->type_id == TypeID<float>::id())
			run_callback = ImGui::InputFloat(field->name.data(), static_cast<float*>(field->data));
		// Vec2
		else if(field->type_id == TypeID<glm::vec2>::id())
			run_callback = ImGui::InputFloat2(field->name.data(), static_cast<float*>(field->data));
		// Vec3
		else if(field->type_id == TypeID<glm::vec3>::id())
			run_callback = ImGui::InputFloat3(field->name.data(), static_cast<float*>(field->data));
		// Vec4 or Quaternion
		else if(field->type_id == TypeID<glm::vec4>::id() || field->type_id == TypeID<glm::quat>::id())
			run_callback = ImGui::InputFloat4(field->name.data(), static_cast<float*>(field->data));
		// String
		else if (field->type_id == TypeID<std::string>::id())
		{
		}

		if (run_callback) field->callback();
	}
}