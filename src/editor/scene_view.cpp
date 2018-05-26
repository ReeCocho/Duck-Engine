/**
 * @file scene_view.cpp
 * @brief Scene view source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\gtc\matrix_transform.hpp>
#include "scene_view.hpp"

namespace dk
{
	SceneView::SceneView(OffScreenForwardRenderer* renderer, EditorRenderer* editor_renderer, Input* input) : 
		m_renderer(renderer),
		m_editor_renderer(editor_renderer),
		m_input(input)
	{
		// Create descriptor pool
		vk::DescriptorPoolSize pool_size = { vk::DescriptorType::eCombinedImageSampler, 1 };

		vk::DescriptorPoolCreateInfo pool_info = {};
		pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		pool_info.maxSets = 1;
		pool_info.poolSizeCount = 1;
		pool_info.pPoolSizes = &pool_size;
		m_vk_descriptor_pool = renderer->get_graphics().get_logical_device().createDescriptorPool(pool_info);
		dk_assert(m_vk_descriptor_pool);

		// Create descriptor set
		vk::DescriptorSetAllocateInfo alloc_info = {};
		alloc_info.descriptorPool = m_vk_descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &editor_renderer->get_descriptor_set_layout();
		m_vk_descriptor_set = renderer->get_graphics().get_logical_device().allocateDescriptorSets(alloc_info)[0];
		dk_assert(m_vk_descriptor_set);

		vk::DescriptorImageInfo image = {};
		image.sampler = renderer->get_color_texture()->get_sampler();
		image.imageView = renderer->get_color_texture()->get_image_view();
		image.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		vk::WriteDescriptorSet write_desc = {};
		write_desc.dstSet = m_vk_descriptor_set;
		write_desc.descriptorCount = 1;
		write_desc.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		write_desc.pImageInfo = &image;
		write_desc.dstBinding = 0;

		renderer->get_graphics().get_logical_device().updateDescriptorSets(write_desc, {});

		// Create camera command buffers
		m_camera.command_buffers =
		{
			m_renderer->get_graphics().get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary),
			m_renderer->get_graphics().get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary)
		};

		// Default camera data
		const glm::mat4 rot_mat = glm::mat4_cast(glm::quat(glm::radians(m_camera.rotation)));
		const glm::vec3 up = glm::vec3(rot_mat[1].x, rot_mat[1].y, rot_mat[1].z);
		const glm::vec3 forward = glm::vec3(rot_mat[2].x, rot_mat[2].y, rot_mat[2].z);
		const float view_height = static_cast<float>(m_renderer->get_color_texture()->get_height());
		const float view_width = static_cast<float>(m_renderer->get_color_texture()->get_width());
		const float view_aspect = view_height / view_width;
		
		CameraData camera_data = {};
		camera_data.vp_mat = generate_vp_matrix(view_aspect, forward, up);
		camera_data.position = m_camera.position;
		camera_data.frustum = Frustum(camera_data.vp_mat);
		camera_data.command_buffers = m_camera.command_buffers;
		m_renderer->set_main_camera(camera_data);
	}

	SceneView::~SceneView()
	{
		// Cleanup
		m_renderer->get_graphics().get_logical_device().destroyDescriptorPool(m_vk_descriptor_pool);

		for (auto& command_buffer : m_camera.command_buffers)
			command_buffer.free();
	}

	void SceneView::draw(float delta_time)
	{
		// Update the descriptor set if needed
		if (m_input->is_resizing())
		{
			vk::DescriptorImageInfo image = {};
			image.sampler = m_renderer->get_color_texture()->get_sampler();
			image.imageView = m_renderer->get_color_texture()->get_image_view();
			image.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			vk::WriteDescriptorSet write_desc = {};
			write_desc.dstSet = m_vk_descriptor_set;
			write_desc.descriptorCount = 1;
			write_desc.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			write_desc.pImageInfo = &image;
			write_desc.dstBinding = 0;

			m_renderer->get_graphics().get_logical_device().updateDescriptorSets(write_desc, {});
		}

		// Get window dimensions and position
		const ImVec2 win_dim = ImGui::GetWindowSize();
		const ImVec2 win_pos = ImGui::GetWindowPos();

		// Get viewport dimensions
		const float view_height = static_cast<float>(m_renderer->get_color_texture()->get_height());
		const float view_width = static_cast<float>(m_renderer->get_color_texture()->get_width());
		const float view_aspect = view_height/view_width;

		// Get horizontal and vertical axis
		const float x = (m_input->get_key_held(dk::KeyCode::A) ? -1.0f : 0.0f) + (m_input->get_key_held(dk::KeyCode::D) ? 1.0f : 0.0f);
		const float y = (m_input->get_key_held(dk::KeyCode::S) ? -1.0f : 0.0f) + (m_input->get_key_held(dk::KeyCode::W) ? 1.0f : 0.0f);

		// Get mouse delta and position
		const glm::vec2 mouse_del = m_input->get_mouse_delta();
		const glm::vec2 mouse_pos = m_input->get_mouse_position();

		// Compute viewport size
		const ImVec2 viewport_size = ImVec2(win_dim.x, win_dim.x * view_aspect);

		if 
		(
			m_input->get_mouse_button_held(MouseButton::Right) && 
			mouse_pos.x > win_pos.x && 
			mouse_pos.x < win_pos.x + viewport_size.x &&
			mouse_pos.y > win_pos.y &&
			mouse_pos.y < win_pos.y + viewport_size.y
		)
		{
			// Rotate camera
			m_camera.rotation.y += mouse_del.x * 0.08f;
			m_camera.rotation.x += mouse_del.y * 0.08f;

			if (m_camera.rotation.x < -90.0f) m_camera.rotation.x = -90.0f;
			else if (m_camera.rotation.x > 90.0f) m_camera.rotation.x = 90.0f;

			// Generate camera data
			const glm::mat4 rot_mat = glm::mat4_cast(glm::quat(glm::radians(m_camera.rotation)));
			const glm::vec3 up = glm::vec3(rot_mat[1].x, rot_mat[1].y, rot_mat[1].z);
			const glm::vec3 right = glm::vec3(rot_mat[0].x, rot_mat[0].y, rot_mat[0].z);
			const glm::vec3 forward = glm::vec3(rot_mat[2].x, rot_mat[2].y, rot_mat[2].z);

			// Move camera
			m_camera.position += forward * y * delta_time * 6.0f;
			m_camera.position += right * x * delta_time * 6.0f;

			// Update camera data
			CameraData camera_data = {};
			camera_data.vp_mat = generate_vp_matrix(view_aspect, forward, up);
			camera_data.position = m_camera.position;
			camera_data.frustum = Frustum(camera_data.vp_mat);
			camera_data.command_buffers = m_camera.command_buffers;
			m_renderer->set_main_camera(camera_data);
		}

		// Begin window
		ImGui::Image(m_vk_descriptor_set, ImVec2(win_dim.x, win_dim.x * view_aspect), ImVec2(1, 1), ImVec2(0, 0));
	}

	glm::mat4 SceneView::generate_vp_matrix(float aspect_ratio, glm::vec3 forward, glm::vec3 up)
	{
		return  glm::perspective(glm::radians(m_camera.field_of_view), 1.0f/aspect_ratio, m_camera.near_clipping, m_camera.far_clipping) *
				glm::lookAt(m_camera.position, m_camera.position + forward, up);
	}
}