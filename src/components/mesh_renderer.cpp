/**
 * @file mesh_renderer.cpp
 * @brief Mesh renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <engine.hpp>
#include "mesh_renderer.hpp"

namespace dk
{
	MeshRenderer::MeshRenderer() : Component<MeshRenderer>(nullptr, Entity()) {}

	void MeshRenderer::generate_resources()
	{
		if (m_material.allocator && m_mesh.allocator)
		{
			// Free old resources
			free_resources();

			// Create buffers
			m_vertex_uniform_buffer = engine::graphics.create_buffer
			(
				m_material->get_shader()->get_inst_vertex_buffer_size(),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			m_fragment_uniform_buffer = engine::graphics.create_buffer
			(
				m_material->get_shader()->get_inst_fragment_buffer_size(),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			vk::DescriptorPoolSize pool_size = {};
			pool_size.type = vk::DescriptorType::eUniformBuffer;
			pool_size.descriptorCount = 4;

			// Create descriptor pool
			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			pool_info.maxSets = 1;

			m_vk_descriptor_pool = engine::graphics.get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_vk_descriptor_pool);

			// Allocate descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_vk_descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_material->get_shader()->get_descriptor_set_layout();

			m_vk_descriptor_set = engine::graphics.get_logical_device().allocateDescriptorSets(alloc_info)[0];
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

			engine::graphics.get_logical_device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
		}
	}

	void MeshRenderer::free_resources()
	{
		if (m_vertex_uniform_buffer.buffer)
		{
			m_vertex_uniform_buffer.free(engine::graphics.get_logical_device());
			m_vertex_uniform_buffer.buffer = vk::Buffer();
		}

		if (m_fragment_uniform_buffer.buffer)
		{
			m_fragment_uniform_buffer.free(engine::graphics.get_logical_device());
			m_fragment_uniform_buffer.buffer = vk::Buffer();
		}

		if (m_vk_descriptor_pool)
		{
			engine::graphics.get_logical_device().destroyDescriptorPool(m_vk_descriptor_pool);
			m_vk_descriptor_pool = vk::DescriptorPool();
			m_vk_descriptor_set = vk::DescriptorSet();
		}
	}

	void MeshRendererSystem::on_begin()
	{
		Handle<MeshRenderer> mesh_renderer = get_component();
		mesh_renderer->m_command_buffer = engine::graphics.get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary);
	}

	void MeshRendererSystem::on_pre_render(float delta_time)
	{
		for (Handle<MeshRenderer> mesh_renderer : *this)
		{
			// Upload vertex shader data
			{
				VertexShaderData v_data = {};
				v_data.mvp = mesh_renderer->mvp;

				void* data = engine::graphics.get_logical_device().mapMemory(mesh_renderer->m_vertex_uniform_buffer.memory, 0, sizeof(VertexShaderData));
				memcpy(data, &v_data, sizeof(VertexShaderData));
				engine::graphics.get_logical_device().unmapMemory(mesh_renderer->m_vertex_uniform_buffer.memory);
			}

			// Upload fragment shader data
			{
				FragmentShaderData f_data = {};

				void* data = engine::graphics.get_logical_device().mapMemory(mesh_renderer->m_fragment_uniform_buffer.memory, 0, sizeof(FragmentShaderData));
				memcpy(data, &f_data, sizeof(FragmentShaderData));
				engine::graphics.get_logical_device().unmapMemory(mesh_renderer->m_fragment_uniform_buffer.memory);
			}

			// Draw
			dk::RenderableObject renderable =
			{
				mesh_renderer->m_command_buffer,
				mesh_renderer->m_material->get_shader(),
				mesh_renderer->m_mesh,
				{ mesh_renderer->m_vk_descriptor_set }
			};
			engine::renderer.draw(renderable);
		}
	}

	void MeshRendererSystem::on_end()
	{
		Handle<MeshRenderer> mesh_renderer = get_component();
		mesh_renderer->m_command_buffer.free();
		mesh_renderer->free_resources();
	}
}