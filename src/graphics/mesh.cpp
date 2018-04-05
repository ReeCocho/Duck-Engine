/**
 * @file mesh.cpp
 * @brief Mesh source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "mesh.hpp"

namespace dk
{
	vk::VertexInputBindingDescription Vertex::get_binding_description()
	{
		vk::VertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = vk::VertexInputRate::eVertex;
		return binding_description;
	}

	std::array<vk::VertexInputAttributeDescription, 2> Vertex::get_attribute_descriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 2> attribute_descriptions = {};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attribute_descriptions[0].offset = offsetof(Vertex, position);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = vk::Format::eR32G32Sfloat;
		attribute_descriptions[1].offset = offsetof(Vertex, uv);
		return attribute_descriptions;
	}



	Mesh::Mesh(Graphics& graphics, const std::vector<unsigned short>& indices, const std::vector<Vertex>& vertices) : 
		m_graphics(graphics),
		m_vertex_buffer({})
	{
		m_index_count = vertices.size();

		// Create vertex buffer
		vk::BufferCreateInfo buffer_info = {};
		buffer_info.size = sizeof(vertices[0]) * vertices.size();
		buffer_info.usage = vk::BufferUsageFlagBits::eVertexBuffer;
		buffer_info.sharingMode = vk::SharingMode::eExclusive;
		m_vertex_buffer.buffer = m_graphics.get_logical_device().createBuffer(buffer_info);
		dk_assert(m_vertex_buffer.buffer);

		// Memory requirements of the buffer
		vk::MemoryRequirements mem_requirements = m_graphics.get_logical_device().getBufferMemoryRequirements(m_vertex_buffer.buffer);

		vk::MemoryAllocateInfo alloc_info = {};
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = find_memory_type
		(
			m_graphics.get_physical_device(), 
			mem_requirements.memoryTypeBits, 
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
		m_vertex_buffer.memory = m_graphics.get_logical_device().allocateMemory(alloc_info);
		dk_assert(m_vertex_buffer.memory);

		// Bind buffer to memory
		m_graphics.get_logical_device().bindBufferMemory(m_vertex_buffer.buffer, m_vertex_buffer.memory, 0);

		// Put vertices into the buffer
		void* data = m_graphics.get_logical_device().mapMemory(m_vertex_buffer.memory, 0, buffer_info.size);
		memcpy(data, vertices.data(), static_cast<size_t>(buffer_info.size));
		m_graphics.get_logical_device().unmapMemory(m_vertex_buffer.memory);
	}

	void Mesh::free()
	{
		m_vertex_buffer.free(m_graphics.get_logical_device());
	}
}