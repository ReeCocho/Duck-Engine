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
		// Get window dimensions
		const float win_height = static_cast<float>(m_renderer->get_graphics().get_height());
		const float win_width = static_cast<float>(m_renderer->get_graphics().get_width());

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

		if (m_input->get_mouse_button_held(MouseButton::Right))
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
		ImGui::SetNextWindowPos(ImVec2(win_width / 4.0f, 0));
		ImGui::SetNextWindowSize(ImVec2(win_width / 2.0f, win_height));
		ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		ImGui::Image(m_vk_descriptor_set, ImVec2(win_width / 2.0f, (win_width/2.0f) * view_aspect), ImVec2(1, 1), ImVec2(0, 0));

		// End window
		ImGui::End();
	}

	glm::mat4 SceneView::generate_vp_matrix(float aspect_ratio, glm::vec3 forward, glm::vec3 up)
	{
		return  glm::perspective(glm::radians(m_camera.field_of_view), 1.0f/aspect_ratio, m_camera.near_clipping, m_camera.far_clipping) *
				glm::lookAt(m_camera.position, m_camera.position + forward, up);
	}
}