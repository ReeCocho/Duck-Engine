/**
 * @file camera.cpp
 * @brief Camera source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\gtc\matrix_transform.hpp>
#include <editor\component_inspector.hpp>
#include "transform.hpp"
#include "camera.hpp"

namespace dk
{
	Handle<Camera> CameraSystem::main_camera = Handle<Camera>(0, nullptr);

	void Camera::calculate_vp_matrices()
	{
		float aspect_ratio = static_cast<float>(engine::graphics.get_width()) / static_cast<float>(engine::graphics.get_height());

		// Create projection frustum
		m_projection = glm::perspective
		(
			glm::radians(m_field_of_view),
			aspect_ratio,
			m_near_clipping_plane,
			m_far_clipping_plane
		);

		// Create view matrix
		m_view = glm::lookAt
		(
			m_transform->get_position(),
			m_transform->get_position() + m_transform->get_forward(),
			-m_transform->get_up()
		);
	}

	void Camera::calculate_frustum()
	{
		// Perspective-view
		m_view_frustum = Frustum(m_projection * m_view);
	}



	void CameraSystem::on_begin()
	{
		Handle<Camera> camera = get_component();
		camera->m_transform = camera->get_entity().get_component<Transform>();
		camera->m_command_buffers =
		{
			engine::graphics.get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary),
			engine::graphics.get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary)
		};
	}

	void CameraSystem::on_pre_render(float delta_time)
	{
		for (Handle<Camera> camera : *this)
		{
			camera->calculate_vp_matrices();
			camera->calculate_frustum();

			if (camera == CameraSystem::main_camera)
			{
				CameraData data = {};
				data.frustum = camera->m_view_frustum;
				data.position = camera->m_transform->get_position();
				data.vp_mat = camera->m_projection * camera->m_view;
				data.sky_box = camera->m_sky_box;
				data.command_buffers = camera->m_command_buffers;

				engine::renderer.set_main_camera(data);
			}
		}
	}

	void CameraSystem::on_end()
	{
		Handle<Camera> camera = get_component();
		for (auto& command_buffer : camera->m_command_buffers)
			command_buffer.free();
	}

	void CameraSystem::serialize(ReflectionContext& archive)
	{
		Handle<Camera> camera = get_component();

		if (auto a = dynamic_cast<ComponentArchive*>(&archive))
		{
			a->field<HSkyBox>(&camera->m_sky_box);
			a->field<float>(&camera->m_field_of_view);
			a->field<float>(&camera->m_near_clipping_plane);
			a->field<float>(&camera->m_far_clipping_plane);

#if !DK_EDITOR
			if (a->is_writing())
			{
				camera->calculate_vp_matrices();
				camera->calculate_frustum();
			}
#endif
		}
		else if (auto a = dynamic_cast<ComponentInspector*>(&archive))
		{
			a->set_name("Camera");
			a->set_field<HSkyBox>("Sky box", &camera->m_sky_box);
			a->set_field<float>("Field of view", &camera->m_field_of_view);
			a->set_field<float>("Near clipping plane", &camera->m_near_clipping_plane);
			a->set_field<float>("Far clipping plane", &camera->m_far_clipping_plane);
		}
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