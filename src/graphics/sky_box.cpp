/**
 * @file sky_box.cpp
 * @brief Sky box class source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "sky_box.hpp"

namespace dk
{
	SkyBox::SkyBox(Graphics* graphics) : 
		m_graphics(graphics)
	{

	}

	void SkyBox::free()
	{
		if (m_vertex_map)
		{
			m_graphics->get_logical_device().unmapMemory(m_vertex_uniform_buffer.memory);
			m_vertex_map = nullptr;
		}

		if (m_fragment_map)
		{
			m_graphics->get_logical_device().unmapMemory(m_fragment_uniform_buffer.memory);
			m_fragment_map = nullptr;
		}

		if (m_vertex_uniform_buffer.buffer)
		{
			m_vertex_uniform_buffer.free(m_graphics->get_logical_device());
			m_vertex_uniform_buffer.buffer = vk::Buffer();
		}

		if (m_fragment_uniform_buffer.buffer)
		{
			m_fragment_uniform_buffer.free(m_graphics->get_logical_device());
			m_fragment_uniform_buffer.buffer = vk::Buffer();
		}

		if (m_vk_descriptor_pool)
		{
			m_graphics->get_logical_device().destroyDescriptorPool(m_vk_descriptor_pool);
			m_vk_descriptor_pool = vk::DescriptorPool();
			m_vk_descriptor_set = vk::DescriptorSet();
		}
	}

	void SkyBox::generate_resources()
	{
		if (m_material.allocator && m_mesh.allocator)
		{
			// Free old resources
			free();

			// Create buffers
			m_vertex_uniform_buffer = m_graphics->create_buffer
			(
				m_material->get_shader()->get_inst_vertex_buffer_size(),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			m_fragment_uniform_buffer = m_graphics->create_buffer
			(
				m_material->get_shader()->get_inst_fragment_buffer_size(),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			// Map buffers
			m_vertex_map = m_graphics->get_logical_device().mapMemory(m_vertex_uniform_buffer.memory, 0, sizeof(VertexShaderData));
			m_fragment_map = m_graphics->get_logical_device().mapMemory(m_fragment_uniform_buffer.memory, 0, sizeof(FragmentShaderData));

			vk::DescriptorPoolSize pool_size = {};
			pool_size.type = vk::DescriptorType::eUniformBuffer;
			pool_size.descriptorCount = 4;

			// Create descriptor pool
			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			pool_info.maxSets = 1;

			m_vk_descriptor_pool = m_graphics->get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_vk_descriptor_pool);

			// Allocate descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_vk_descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_material->get_shader()->get_descriptor_set_layout();

			m_vk_descriptor_set = m_graphics->get_logical_device().allocateDescriptorSets(alloc_info)[0];
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

			m_graphics->get_logical_device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
		}
	}

	Handle<Mesh> SkyBox::set_mesh(Handle<Mesh> mesh)
	{
		m_mesh = mesh;
		generate_resources();
		return m_mesh;
	}

	Handle<Material> SkyBox::set_material(Handle<Material> material)
	{
		m_material = material;
		generate_resources();
		return m_material;
	}

	void SkyBox::set_vertex_data(const VertexShaderData& data)
	{
		memcpy(m_vertex_map, &data, sizeof(VertexShaderData));
	}
}