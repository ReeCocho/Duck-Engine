#pragma once

/**
 * @file mesh.hpp
 * @brief Container of mesh data.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <utilities\frustum.hpp>
#include "graphics.hpp"

namespace dk
{
	/**
	 * @brief Vertex informaton.
	 */
	struct Vertex
	{
		/** Position. */
		glm::vec3 position = {};

		/** UV coordinate. */
		glm::vec2 uv = {};

		/** Normal. */
		glm::vec3 normal = {};

		/** Tangent. */
		glm::vec3 tangent = {};

		/**
		 * @brief Description of vertex bindings used by Vulkan.
		 * @note Binding description.
		 */
		static vk::VertexInputBindingDescription get_binding_description();

		/**
		 * @brief Array of descriptions for each vertex input.
		 * @note Vertex input descriptions.
		 */
		static std::array<vk::VertexInputAttributeDescription, 4> get_attribute_descriptions();

		/**
		 * @brief Equivilence operator.
		 * @param Other vertex.
		 * @return If the vertices are equal.
		 */
		bool operator==(const Vertex& other) const
		{
			return position == other.position && uv == other.uv && normal == other.normal && tangent == other.tangent;
		}

		/**
		 * @brief Unequivilence operator.
		 * @param Other vertex.
		 * @return If the vertices are not equal.
		 */
		bool operator!=(const Vertex& other) const
		{
			return position != other.position || uv != other.uv || normal != other.normal || tangent != other.tangent;
		}
	};



	/**
	 * @brief Container of mesh data.
	 */
	class Mesh
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Mesh();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Indices.
		 * @param Vertices.
		 * @note Tangents will be calculated, so don't worry about doing that yourself.
		 */
		Mesh(Graphics* graphics, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);

		/**
		 * @brief Destructor.
		 */
		~Mesh() = default;
		
		/**
		 * @brief Free mesh data.
		 * @note Used internally. Do not call.
		 */
		void free();

		/**
		 * @brief Get vertex memory buffer.
		 * @return Vertex memory buffer.
		 */
		const VkMemBuffer& get_vertex_buffer() const
		{
			return m_vertex_buffer;
		}

		/**
		 * @brief Get index memory buffer.
		 * @return Index memory buffer.
		 */
		const VkMemBuffer& get_index_buffer() const
		{
			return m_index_buffer;
		}

		/**
		 * @brief Get index count.
		 * @return Index count.
		 */
		size_t get_index_count() const
		{
			return m_indices.size();
		}

		/**
		 * @brief Get AABB bounding box.
		 * @return AABB.
		 */
		AABB get_aabb() const
		{
			return m_aabb;
		}

		/**
		 * @brief Compute normals.
		 */
		void compute_normals();

	private:

		/**
		 * @brief Initialize the index buffer.
		 */
		void init_index_buffer();

		/**
		 * @brief Initialize the vertex buffer.
		 */
		void init_vertex_buffer();

		/**
		 * @brief Calculate tangents and reinitialize buffers.
		 */
		void calculate_tangents();

		/**
		 * @brief Calculate AABB bounding box.
		 */
		void calculate_aabb();



		/** Graphics context. */
		Graphics* m_graphics;

		/** Vertex buffer. */
		VkMemBuffer m_vertex_buffer;

		/** Index buffer. */
		VkMemBuffer m_index_buffer;

		/** Indices. */
		std::vector<uint16_t> m_indices = {};

		/** Vertices. */
		std::vector<Vertex> m_vertices = {};

		/** AABB box. */
		AABB m_aabb = {};
	};
}