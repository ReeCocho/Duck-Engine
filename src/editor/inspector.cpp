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

		// Find component fields
		for (size_t i = 0; i < m_scene->get_system_count(); ++i)
		{
			auto archive = std::make_unique<ReflectionContext>();

			// Try to inspect component
			if (m_scene->get_system_by_index(i)->inspect_by_entity(*archive, m_inspected_entity))
				m_component_inspectors.push_back(std::move(archive));
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
				// Show component name
				if (ImGui::CollapsingHeader(archive->get_name().data()))
				{
					const auto& fields = archive->get_fields();

					// Loop over every field
					for (const auto& field : fields)
						draw_field(field.get());
				}
			}
		}
	}

	void Inspector::draw_field(ReflectionContext::Field* field)
	{
		bool run_callback = false;

		// Float
		if (field->type_id == TypeID<float>::id())
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