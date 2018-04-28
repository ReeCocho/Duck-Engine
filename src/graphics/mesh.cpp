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

	std::array<vk::VertexInputAttributeDescription, 4> Vertex::get_attribute_descriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 4> attribute_descriptions = {};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attribute_descriptions[0].offset = offsetof(Vertex, position);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = vk::Format::eR32G32Sfloat;
		attribute_descriptions[1].offset = offsetof(Vertex, uv);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = vk::Format::eR32G32B32Sfloat;
		attribute_descriptions[2].offset = offsetof(Vertex, normal);

		attribute_descriptions[3].binding = 0;
		attribute_descriptions[3].location = 3;
		attribute_descriptions[3].format = vk::Format::eR32G32B32Sfloat;
		attribute_descriptions[3].offset = offsetof(Vertex, tangent);
		return attribute_descriptions;
	}



	Mesh::Mesh() {}

	Mesh::Mesh(Graphics* graphics, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices) :
		m_graphics(graphics),
		m_vertex_buffer({}),
		m_indices(indices),
		m_vertices(vertices)
	{
		calculate_aabb();
		calculate_tangents();
	}

	void Mesh::free()
	{
		m_vertex_buffer.free(m_graphics->get_logical_device());
		m_index_buffer.free(m_graphics->get_logical_device());
	}

	void Mesh::init_index_buffer()
	{
		vk::DeviceSize buffer_size = sizeof(m_indices[0]) * m_indices.size();

		// Create staging buffer
		VkMemBuffer staging_buffer = m_graphics->create_buffer
		(
			buffer_size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		// Put indices into the bfufer
		void* data = m_graphics->get_logical_device().mapMemory(staging_buffer.memory, 0, buffer_size);
		memcpy(data, m_indices.data(), static_cast<size_t>(buffer_size));
		m_graphics->get_logical_device().unmapMemory(staging_buffer.memory);

		// Create index buffer
		m_index_buffer = m_graphics->create_buffer
		(
			buffer_size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		// Copy data from staging buffer into the index buffer
		m_graphics->copy_buffer(staging_buffer.buffer, m_index_buffer.buffer, buffer_size);

		// Free the staging buffer
		staging_buffer.free(m_graphics->get_logical_device());
	}

	void Mesh::init_vertex_buffer()
	{
		vk::DeviceSize buffer_size = sizeof(m_vertices[0]) * m_vertices.size();

		// Create staging buffer
		VkMemBuffer staging_buffer = m_graphics->create_buffer
		(
			buffer_size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		// Put vertices into the buffer
		void* data = m_graphics->get_logical_device().mapMemory(staging_buffer.memory, 0, buffer_size);
		memcpy(data, m_vertices.data(), static_cast<size_t>(buffer_size));
		m_graphics->get_logical_device().unmapMemory(staging_buffer.memory);

		// Create vertex buffer
		m_vertex_buffer = m_graphics->create_buffer
		(
			buffer_size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		// Copy data from staging buffer into the vertex buffer
		m_graphics->copy_buffer(staging_buffer.buffer, m_vertex_buffer.buffer, buffer_size);

		// Free the staging buffer
		staging_buffer.free(m_graphics->get_logical_device());
	}

	void Mesh::calculate_tangents()
	{
		// Free buffers if needed
		if (m_index_buffer.buffer && m_index_buffer.memory)
			m_index_buffer.free(m_graphics->get_logical_device());

		if (m_vertex_buffer.buffer && m_vertex_buffer.memory)
			m_vertex_buffer.free(m_graphics->get_logical_device());

		for (size_t i = 0; i < m_indices.size(); i += 3)
		{
			Vertex& v0 = m_vertices[m_indices[i]];
			Vertex& v1 = m_vertices[m_indices[i + 1]];
			Vertex& v2 = m_vertices[m_indices[i + 2]];

			const auto edge1 = v1.position - v0.position;
			const auto edge2 = v2.position - v0.position;

			const auto deltaU1 = v1.uv.x - v0.uv.x;
			const auto deltaV1 = v1.uv.y - v0.uv.y;
			const auto deltaU2 = v2.uv.x - v0.uv.x;
			const auto deltaV2 = v2.uv.y - v0.uv.y;

			const auto f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

			glm::vec3 tangent = glm::vec3();

			tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
			tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
			tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

			v0.tangent += tangent;
			v1.tangent += tangent;
			v2.tangent += tangent;
		}

		for (size_t i = 0; i < m_vertices.size(); ++i)
			m_vertices[i].tangent = glm::normalize(m_vertices[i].tangent);

		init_vertex_buffer();
		init_index_buffer();
	}

	void Mesh::calculate_aabb()
	{
		// Reset bounding box
		glm::vec3 min = {}, max = {};

		// Loop over every vertex finding the min and max values.
		for (auto& vertex : m_vertices)
		{
			if (vertex.position.x < min.x) min.x = vertex.position.x;
			if (vertex.position.x > max.x) max.x = vertex.position.x;
								    	   
			if (vertex.position.y < min.y) min.y = vertex.position.y;
			if (vertex.position.y > max.y) max.y = vertex.position.y;
								    	   
			if (vertex.position.z < min.z) min.z = vertex.position.z;
			if (vertex.position.z > max.z) max.z = vertex.position.z;
		}

		m_aabb.center = (min + max) / 2.0f;
		m_aabb.extent = (max - min) / 2.0f;
	}
}