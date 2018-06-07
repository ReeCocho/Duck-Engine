/**
* @file transform.cpp
* @brief Transform source file.
* @author Connor J. Bramham (ReeCocho)
*/

/** Includes. */
#include <algorithm>
#include <glm\gtc\matrix_transform.hpp>
#include <editor\component_inspector.hpp>
#include "transform.hpp"

namespace dk
{
	glm::vec3 Transform::set_position(glm::vec3 value)
	{
		m_position = value;
		global_to_local_position();
		generate_model_matrix();
		update_children();

		return m_position;
	}

	glm::vec3 Transform::set_local_position(glm::vec3 value)
	{
		m_local_position = value;
		local_to_global_position();
		generate_model_matrix();
		update_children();

		return m_local_position;
	}

	glm::quat Transform::set_rotation(glm::quat value)
	{
		m_rotation = value;
		m_euler_angles = glm::degrees(glm::eulerAngles(value));
		global_to_local_rotation();
		global_to_local_euler_angles();
		generate_model_matrix();
		update_children();

		return m_rotation;
	}

	glm::quat Transform::set_local_rotation(glm::quat value)
	{
		m_local_rotation = value;
		m_local_euler_angles = glm::degrees(glm::eulerAngles(value));
		local_to_global_rotation();
		local_to_global_euler_angles();
		generate_model_matrix();
		update_children();

		return m_local_rotation;
	}

	glm::vec3 Transform::set_euler_angles(glm::vec3 value)
	{
		value.x = std::fmod(value.x, 360.0f);
		value.y = std::fmod(value.y, 360.0f);
		value.z = std::fmod(value.z, 360.0f);

		m_euler_angles = value;
		m_rotation = glm::quat(value * (glm::pi<float>() / 180.0f));
		global_to_local_euler_angles();
		global_to_local_rotation();
		generate_model_matrix();
		update_children();

		return m_euler_angles;
	}

	glm::vec3 Transform::set_local_euler_angles(glm::vec3 value)
	{
		value.x = std::fmod(value.x, 360.0f);
		value.y = std::fmod(value.y, 360.0f);
		value.z = std::fmod(value.z, 360.0f);

		m_local_euler_angles = value;
		m_local_rotation = glm::quat(glm::radians(value));
		local_to_global_euler_angles();
		local_to_global_rotation();
		generate_model_matrix();
		update_children();

		return m_local_euler_angles;
	}

	glm::vec3 Transform::set_local_scale(glm::vec3 value)
	{
		m_local_scale = value;
		generate_model_matrix();
		update_children();
		return m_local_scale;
	}

	Handle<Transform> Transform::set_parent(Handle<Transform> parent, bool maintain_local)
	{
		// Make sure we don't parent to ourselves
		if (parent == get_handle())
			return m_parent;

		// Remove self from parents child list
		if (m_parent != Handle<Transform>())
			m_parent->m_children.erase(std::remove(m_parent->m_children.begin(), m_parent->m_children.end(), get_handle()), m_parent->m_children.end());

		// Set parent
		m_parent = parent;

		// Add self to new parents children list
		if (m_parent != Handle<Transform>())
			m_parent->m_children.push_back(get_handle());

		// Update transformation
		if (maintain_local)
		{
			local_to_global_position();
			local_to_global_rotation();
			local_to_global_euler_angles();
		}
		else
		{
			global_to_local_position();
			global_to_local_rotation();
			global_to_local_euler_angles();
		}

		// Update local values and model matrix
		generate_model_matrix();
		update_children();

		return m_parent;
	}

	void Transform::generate_model_matrix()
	{
		// Reset matrices
		m_model_matrix = {};
		m_unscaled_model_matrix = {};

		// Translation matrix
		m_model_matrix = glm::translate(m_model_matrix, m_local_position);

		// Rotation matrix
		m_model_matrix *= glm::mat4_cast(m_local_rotation);
		m_unscaled_model_matrix = m_model_matrix;

		// Scale matrix
		m_model_matrix = glm::scale(m_model_matrix, m_local_scale);

		// Get parent matrix if we have one
		if (m_parent != Handle<Transform>())
		{
			m_model_matrix = m_parent->m_unscaled_model_matrix * m_model_matrix;
			m_unscaled_model_matrix = m_parent->m_unscaled_model_matrix * m_unscaled_model_matrix;
		}

		// Update childrens model matrix
		for (auto child : m_children)
			child->generate_model_matrix();
	}

	void Transform::update_children()
	{
		for (auto child : m_children)
		{
			// Update global transformations
			child->local_to_global_position();
			child->local_to_global_rotation();
			child->local_to_global_euler_angles();

			// Update model matrix
			child->generate_model_matrix();
			child->update_children();
		}
	}

	void Transform::global_to_local_position()
	{
		if (m_parent == Handle<Transform>()) m_local_position = m_position;
		else
		{
			glm::vec4 new_pos = glm::inverse(m_parent->m_unscaled_model_matrix) * glm::vec4(m_position, 1.0);
			m_local_position = glm::vec3(new_pos.x, new_pos.y, new_pos.z);
		}
	}

	void Transform::global_to_local_rotation()
	{
		if (m_parent == Handle<Transform>()) m_local_rotation = m_rotation;
		else m_local_rotation = inverse(m_parent->m_rotation) * m_rotation;
	}

	void Transform::global_to_local_euler_angles()
	{
		if (m_parent == Handle<Transform>()) m_local_euler_angles = m_euler_angles;
		else m_local_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));
	}

	void Transform::local_to_global_position()
	{
		if (m_parent == Handle<Transform>()) m_position = m_local_position;
		else
		{
			glm::vec4 new_pos = m_parent->m_unscaled_model_matrix * glm::vec4(m_local_position, 1.0);
			m_position = glm::vec3(new_pos.x, new_pos.y, new_pos.z);
		}
	}

	void Transform::local_to_global_rotation()
	{
		if (m_parent == Handle<Transform>()) m_rotation = m_local_rotation;
		else m_rotation = m_parent->m_rotation * m_local_rotation;
	}

	void Transform::local_to_global_euler_angles()
	{
		if (m_parent == Handle<Transform>()) m_euler_angles = m_local_euler_angles;
		else m_euler_angles = glm::degrees(glm::eulerAngles(m_rotation));
	}



	void TransformSystem::on_new_entity(Entity entity)
	{
		entity.add_component<Transform>();
	}

	void TransformSystem::on_begin()
	{
		get_component()->generate_model_matrix();
		get_component()->update_children();
	}

	void TransformSystem::on_end()
	{
		auto transform = get_component();

		while(transform->child_count() > 0)
			transform->get_child(0)->set_parent(Handle<Transform>(0, nullptr));

		transform->set_parent(Handle<Transform>(0, nullptr));
	}

	void TransformSystem::serialize(ReflectionContext& archive)
	{
		Handle<Transform> transform = get_component();

		if (auto a = dynamic_cast<ComponentArchive*>(&archive))
		{
			a->field(transform->m_local_position);
			a->field(transform->m_local_rotation);
			a->field(transform->m_local_scale);
			a->field(transform->m_parent);
			a->field(transform->m_children);

			if (!a->is_writing())
			{
				transform->set_local_position(transform->m_local_position);
				transform->set_local_rotation(transform->m_local_rotation);
				transform->set_local_scale(transform->m_local_scale);
			}
		}
		else if (auto a = dynamic_cast<ComponentInspector*>(&archive))
		{
			a->set_name("Transform");
			a->set_field("Position", &transform->m_local_position, [transform] { transform->generate_model_matrix(); });
			a->set_field("Rotation", &transform->m_local_euler_angles, [transform] { transform->set_local_euler_angles(transform->get_local_euler_angles()); });
			a->set_field("Scale", &transform->m_local_scale, [transform] { transform->generate_model_matrix(); });
		}
	}
}