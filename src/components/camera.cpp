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



	void CameraSystem::on_begin()
	{
		Handle<Camera> camera = get_component();
		camera->m_transform = camera->get_entity().get_component<Transform>();
		camera->m_virtual_camera = dk::engine::renderer.create_camera();
	}

	void CameraSystem::on_pre_render(float delta_time)
	{
		for (Handle<Camera> camera : *this)
		{
			camera->m_proj_view = glm::perspective
			(
				glm::radians(camera->m_field_of_view),
				static_cast<float>(engine::graphics.get_width())/static_cast<float>(engine::graphics.get_height()),
				camera->m_near_clipping_plane,
				camera->m_far_clipping_plane
			) * 
			glm::lookAt
			(
				camera->m_transform->get_position(), 
				camera->m_transform->get_position() + camera->m_transform->get_forward(), 
				camera->m_transform->get_up()
			);
		}
	}

	void CameraSystem::on_end()
	{
		Handle<Camera> camera = get_component();
		dk::engine::renderer.destroy_camera(camera->m_virtual_camera);
	}

	void CameraSystem::set_main_camera(Handle<Camera> camera)
	{
		if (camera.allocator)
			dk::engine::renderer.set_main_camera(camera->m_virtual_camera);
		else
			dk::engine::renderer.set_main_camera(Handle<VirtualCamera>(0, nullptr));

		main_camera = camera;
	}

	Handle<Camera> CameraSystem::get_main_camera()
	{
		return main_camera;
	}
}