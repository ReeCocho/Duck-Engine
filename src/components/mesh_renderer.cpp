/**
 * @file mesh_renderer.cpp
 * @brief Mesh renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <engine/common.hpp>
#include "transform.hpp"
#include <engine/config.hpp>
#include "camera.hpp"
#include "mesh_renderer.hpp"

namespace dk
{
#if DK_EDITOR
	namespace active_system = editor;
#else
	namespace active_system = engine;
#endif

	void MeshRenderer::generate_resources()
	{
		if (m_material.allocator && m_mesh.allocator)
		{
			// Free old resources
			free_resources();

			// Create buffers
			m_vertex_uniform_buffer = active_system::graphics.create_buffer
			(
				m_material->get_shader()->get_inst_vertex_buffer_size(),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			m_fragment_uniform_buffer = active_system::graphics.create_buffer
			(
				m_material->get_shader()->get_inst_fragment_buffer_size(),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			// Map buffers
			m_vertex_map = active_system::graphics.get_logical_device().mapMemory(m_vertex_uniform_buffer.memory, 0, sizeof(VertexShaderData));
			m_fragment_map = active_system::graphics.get_logical_device().mapMemory(m_fragment_uniform_buffer.memory, 0, sizeof(FragmentShaderData));

			vk::DescriptorPoolSize pool_size = {};
			pool_size.type = vk::DescriptorType::eUniformBuffer;
			pool_size.descriptorCount = 4;

			// Create descriptor pool
			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			pool_info.maxSets = 1;

			m_vk_descriptor_pool = active_system::graphics.get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_vk_descriptor_pool);

			// Allocate descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_vk_descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_material->get_shader()->get_descriptor_set_layout();

			m_vk_descriptor_set = active_system::graphics.get_logical_device().allocateDescriptorSets(alloc_info)[0];
			dk_assert(m_vk_descriptor_set);

			// Update descriptor set
			std::array<vk::DescriptorBufferInfo, 4> buffer_infos = {};
			buffer_infos[0].buffer = m_material->get_vertex_uniform_buffer().buffer;
			buffer_infos[0].offset = 0;
			buffer_infos[0].range = m_material->get_shader()->get_vertex_buffer_size();

			buffer_infos[1].buffer = m_vertex_uniform_buffer.buffer;
			buffer_infos[1].offset = 0;
			buffer_infos[1].range = m_material->get_shader()->get_inst_vertex_buffer_size();

			buffer_infos[2].buffer = m_material->get_fragment_uniform_buffer().buffer;
			buffer_infos[2].offset = 0;
			buffer_infos[2].range = m_material->get_shader()->get_fragment_buffer_size();

			buffer_infos[3].buffer = m_fragment_uniform_buffer.buffer;
			buffer_infos[3].offset = 0;
			buffer_infos[3].range = m_material->get_shader()->get_inst_fragment_buffer_size();

			std::array<vk::WriteDescriptorSet, 4> writes = {};

			for (size_t i = 0; i < 4; ++i)
			{
				writes[i].dstSet = m_vk_descriptor_set;
				writes[i].dstBinding = static_cast<uint32_t>(i);
				writes[i].dstArrayElement = 0;
				writes[i].descriptorType = vk::DescriptorType::eUniformBuffer;
				writes[i].descriptorCount = 1;
				writes[i].pBufferInfo = &buffer_infos[i];
			}

			active_system::graphics.get_logical_device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
		}
	}

	void MeshRenderer::free_resources()
	{
		if (m_vertex_map)
		{
			active_system::graphics.get_logical_device().unmapMemory(m_vertex_uniform_buffer.memory);
			m_vertex_map = nullptr;
		}

		if (m_fragment_map)
		{
			active_system::graphics.get_logical_device().unmapMemory(m_fragment_uniform_buffer.memory);
			m_fragment_map = nullptr;
		}

		if (m_vertex_uniform_buffer.buffer)
		{
			m_vertex_uniform_buffer.free(active_system::graphics.get_logical_device());
			m_vertex_uniform_buffer.buffer = vk::Buffer();
		}

		if (m_fragment_uniform_buffer.buffer)
		{
			m_fragment_uniform_buffer.free(active_system::graphics.get_logical_device());
			m_fragment_uniform_buffer.buffer = vk::Buffer();
		}

		if (m_vk_descriptor_pool)
		{
			active_system::graphics.get_logical_device().destroyDescriptorPool(m_vk_descriptor_pool);
			m_vk_descriptor_pool = vk::DescriptorPool();
			m_vk_descriptor_set = vk::DescriptorSet();
		}
	}

	void MeshRendererSystem::on_begin()
	{
		Handle<MeshRenderer> mesh_renderer = get_component();
		mesh_renderer->m_transform = mesh_renderer->get_entity().get_component<Transform>();
		mesh_renderer->m_command_buffer = active_system::graphics.get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary);
		mesh_renderer->m_depth_prepass_command_buffer = active_system::graphics.get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary);
		mesh_renderer->generate_resources();
	}

	void MeshRendererSystem::on_pre_render(float delta_time)
	{
#if !DK_EDITOR
		if (!CameraSystem::get_main_camera().allocator)
			return;
#endif

		for (Handle<MeshRenderer> mesh_renderer : *this)
		{
			if (
				!mesh_renderer->m_mesh.allocator || 
				!mesh_renderer->m_material.allocator || 
				!mesh_renderer->m_fragment_map || 
				!mesh_renderer->m_vertex_map
				)
				continue;

			bool textures_unbound = false;
			for (size_t i = 0; i < mesh_renderer->m_material->get_shader()->get_texture_count(); ++i)
				if (!mesh_renderer->m_material->get_texture(i).allocator)
				{
					textures_unbound = true;
					break;
				}

			if (textures_unbound)
				continue;

			// Upload vertex shader data
			{
				VertexShaderData v_data = {};
				v_data.model = mesh_renderer->m_transform->get_model_matrix();
#if DK_EDITOR
				v_data.mvp = editor::renderer.get_main_camera().vp_mat * mesh_renderer->m_transform->get_model_matrix();
#else
				v_data.mvp = CameraSystem::get_main_camera()->get_pv_matrix() * mesh_renderer->m_transform->get_model_matrix();
#endif
				memcpy(mesh_renderer->m_vertex_map, &v_data, sizeof(VertexShaderData));
			}

			// Upload fragment shader data
			{
				FragmentShaderData f_data = {};
				memcpy(mesh_renderer->m_fragment_map, &f_data, sizeof(FragmentShaderData));
			}

			// Draw
			dk::RenderableObject renderable =
			{
				{
					mesh_renderer->m_command_buffer,
					mesh_renderer->m_depth_prepass_command_buffer
				},
				mesh_renderer->m_material->get_shader(),
				mesh_renderer->m_mesh,
				{ mesh_renderer->m_vk_descriptor_set, dk::active_system::renderer.get_descriptor_set() },
				mesh_renderer->m_transform->get_model_matrix()
			};

			if (mesh_renderer->m_material->get_shader()->get_texture_count() > 0)
				renderable.descriptor_sets.push_back(mesh_renderer->m_material->get_texture_descriptor_set());

			active_system::renderer.draw(renderable);
		}
	}

	void MeshRendererSystem::on_end()
	{
		Handle<MeshRenderer> mesh_renderer = get_component();
		mesh_renderer->m_command_buffer.free();
		mesh_renderer->m_depth_prepass_command_buffer.free();
		mesh_renderer->free_resources();
	}

	void MeshRendererSystem::serialize(ComponentArchive& archive)
	{
		Handle<MeshRenderer> mesh_renderer = get_component();
		archive.set_name("Mesh Renderer");
		archive.set_field("Mesh", mesh_renderer->m_mesh);
		archive.set_field("Material", mesh_renderer->m_material);
	}

	void MeshRendererSystem::inspect(ReflectionContext& context)
	{
		Handle<MeshRenderer> mesh_renderer = get_component();
		context.set_name("Mesh Renderer");
		context.set_field("Mesh", mesh_renderer->m_mesh);
		context.set_field("Material", mesh_renderer->m_material);
	}
}