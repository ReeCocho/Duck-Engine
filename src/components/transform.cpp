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

		if (m_parent == Handle<Transform>(0, nullptr))
			m_local_position = m_position;
		else
		{
			glm::vec4 newPos = glm::inverse(m_parent->m_unscaled_model_matrix) * glm::vec4(m_position, 1.0);
			m_local_position = glm::vec3(newPos.x, newPos.y, newPos.z);
		}

		generate_model_matrix();
		update_children();

		return m_position;
	}

	glm::vec3 Transform::set_local_position(glm::vec3 value)
	{
		m_local_position = value;

		if (m_parent == Handle<Transform>(0, nullptr))
			m_position = value;
		else
		{
			glm::vec4 newPos = m_parent->m_unscaled_model_matrix * glm::vec4(m_local_position, 1.0);
			m_position = glm::vec3(newPos.x, newPos.y, newPos.z);
		}

		generate_model_matrix();
		update_children();

		return m_local_position;
	}

	glm::quat Transform::set_rotation(glm::quat value)
	{
		m_rotation = value;
		m_euler_angles = glm::degrees(glm::eulerAngles(value));

		if (m_parent == Handle<Transform>(0, nullptr))
		{
			m_local_rotation = m_rotation;
			m_local_euler_angles = m_euler_angles;
		}
		else
		{
			m_local_rotation = inverse(m_parent->m_rotation) * m_rotation;
			m_local_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));
		}

		generate_model_matrix();
		update_children();

		return m_rotation;
	}

	glm::quat Transform::set_local_rotation(glm::quat value)
	{
		m_local_rotation = value;
		m_local_euler_angles = glm::degrees(glm::eulerAngles(value));

		if (m_parent == Handle<Transform>(0, nullptr))
		{
			m_rotation = m_local_rotation;
			m_euler_angles = m_local_euler_angles;
		}
		else
		{
			m_rotation = m_parent->m_rotation * m_local_rotation;
			m_euler_angles = glm::degrees(glm::eulerAngles(m_rotation));
		}

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
		
		if (m_parent == Handle<Transform>(0, nullptr))
		{
			m_local_rotation = m_rotation;
			m_local_euler_angles = m_euler_angles;
		}
		else
		{
			m_local_rotation = glm::inverse(m_parent->m_rotation) * m_rotation;
			m_local_euler_angles = glm::degrees(glm::eulerAngles(m_local_rotation));
		}
		
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

		if (m_parent == Handle<Transform>(0, nullptr))
		{
			m_euler_angles = m_local_euler_angles;
			m_rotation = m_local_rotation;
		}
		else
		{
			m_rotation = m_parent->m_rotation * m_local_rotation;
			m_euler_angles = glm::degrees(glm::eulerAngles(m_rotation));
		}

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

	Handle<Transform> Transform::set_parent(Handle<Transform> parent)
	{
		// Remove self from parents child list
		if (m_parent != Handle<Transform>(0, nullptr))
			m_parent->m_children.erase(std::remove(m_parent->m_children.begin(), m_parent->m_children.end(), get_handle()), m_parent->m_children.end());

		// Set parent
		m_parent = parent;

		// Add self to new parents children list
		if (m_parent != Handle<Transform>(0, nullptr))
			m_parent->m_children.push_back(get_handle());

		// Update local values and model matrix
		generate_model_matrix();

		return m_parent;
	}

	void Transform::generate_model_matrix()
	{
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
		if (m_parent != Handle<Transform>(0, nullptr))
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
			// Update rotation
			child->m_rotation = m_rotation * child->m_local_rotation;
			child->m_euler_angles = glm::degrees(glm::eulerAngles(m_rotation));

			// Update position
			glm::vec4 newPos = m_unscaled_model_matrix * glm::vec4(child->m_local_position, 1.0);
			child->m_position = glm::vec3(newPos.x, newPos.y, newPos.z);

			// Update model matrix and childrens children
			child->generate_model_matrix();
			child->update_children();
		}
	}



	void TransformSystem::on_new_entity(Entity entity)
	{
		entity.add_component<Transform>();
	}

	void TransformSystem::on_begin()
	{
		get_component()->generate_model_matrix();
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
			a->field<glm::vec3>(&transform->m_local_position);
			a->field<glm::quat>(&transform->m_local_rotation);
			a->field<glm::vec3>(&transform->m_local_scale);
		}
		else if (auto a = dynamic_cast<ComponentInspector*>(&archive))
		{
			a->set_name("Transform");
			a->set_field<glm::vec3>("Position", &transform->m_local_position, [transform] { transform->generate_model_matrix(); });
			a->set_field<glm::vec3>("Rotation", &transform->m_local_euler_angles, [transform] { transform->set_local_euler_angles(transform->get_local_euler_angles()); });
			a->set_field<glm::vec3>("Scale", &transform->m_local_scale, [transform] { transform->generate_model_matrix(); });
		}
	}
}