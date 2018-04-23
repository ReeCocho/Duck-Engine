/**
 * @file camera.cpp
 * @brief Camera source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\gtc\matrix_transform.hpp>
#include <ecs\transform.hpp>
#include "camera.hpp"

namespace dk
{
	Handle<Camera> CameraSystem::main_camera = Handle<Camera>(0, nullptr);

	bool Camera::check_inside_frustum(glm::vec3 p)
	{
		// Point in clip space
		glm::vec4 p_clip = m_projection * m_view * glm::vec4(p, 1.0f);
		return	glm::abs(p_clip.x) < p_clip.w &&
				glm::abs(p_clip.y) < p_clip.w &&
				0.0f < p_clip.z &&
				p_clip.z < p_clip.w;
	}

	// TODO: Implementation
	bool Camera::check_inside_frustum(glm::vec3 c, float r)
	{
		return false;
	}



	void CameraSystem::on_begin()
	{
		Handle<Camera> camera = get_component();
		camera->m_transform = camera->get_entity().get_component<Transform>();
	}

	void CameraSystem::on_pre_render(float delta_time)
	{
		for (Handle<Camera> camera : *this)
		{
			float aspect_ratio = static_cast<float>(engine::graphics.get_width()) / static_cast<float>(engine::graphics.get_height());

			// Create projection frustum
			camera->m_projection = glm::perspective
			(
				glm::radians(camera->m_field_of_view),
				aspect_ratio,
				camera->m_near_clipping_plane,
				camera->m_far_clipping_plane
			);

			// Create view matrix
			camera->m_view = glm::lookAt
			(
				camera->m_transform->get_position(), 
				camera->m_transform->get_position() + camera->m_transform->get_forward(), 
				-camera->m_transform->get_up()
			);

			if (camera == CameraSystem::main_camera)
				engine::renderer.set_main_camera_matrix(camera->m_projection * camera->m_view);
		}
	}

	void CameraSystem::on_end()
	{
		Handle<Camera> camera = get_component();
	}

	void CameraSystem::set_main_camera(Handle<Camera> camera)
	{
		main_camera = camera;
	}

	Handle<Camera> CameraSystem::get_main_camera()
	{
		return main_camera;
	}
}