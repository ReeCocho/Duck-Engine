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



	Mesh::Mesh(Graphics& graphics, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices) :
		m_graphics(graphics),
		m_vertex_buffer({}),
		m_index_count(indices.size())
	{
		{
			vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

			// Create staging buffer
			VkMemBuffer staging_buffer = m_graphics.create_buffer
			(
				buffer_size,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			// Put vertices into the buffer
			void* data = m_graphics.get_logical_device().mapMemory(staging_buffer.memory, 0, buffer_size);
			memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
			m_graphics.get_logical_device().unmapMemory(staging_buffer.memory);

			// Create vertex buffer
			m_vertex_buffer = m_graphics.create_buffer
			(
				buffer_size,
				vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

			// Copy data from staging buffer into the vertex buffer
			m_graphics.copy_buffer(staging_buffer.buffer, m_vertex_buffer.buffer, buffer_size);

			// Free the staging buffer
			staging_buffer.free(m_graphics.get_logical_device());
		}

		{
			vk::DeviceSize buffer_size = sizeof(indices[0]) * indices.size();

			// Create staging buffer
			VkMemBuffer staging_buffer = m_graphics.create_buffer
			(
				buffer_size,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);

			// Put indices into the bfufer
			void* data = m_graphics.get_logical_device().mapMemory(staging_buffer.memory, 0, buffer_size);
			memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
			m_graphics.get_logical_device().unmapMemory(staging_buffer.memory);

			// Create index buffer
			m_index_buffer = m_graphics.create_buffer
			(
				buffer_size,
				vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

			// Copy data from staging buffer into the index buffer
			m_graphics.copy_buffer(staging_buffer.buffer, m_index_buffer.buffer, buffer_size);

			// Free the staging buffer
			staging_buffer.free(m_graphics.get_logical_device());
		}
	}

	void Mesh::free()
	{
		m_vertex_buffer.free(m_graphics.get_logical_device());
		m_index_buffer.free(m_graphics.get_logical_device());
	}
}