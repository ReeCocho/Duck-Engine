#pragma once

/**
 * @file mesh.hpp
 * @brief Container of mesh data.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <math\common.hpp>
#include "graphics.hpp"

namespace dk
{
	/**
	 * @brief Vertex informaton.
	 */
	struct Vertex
	{
		/** Position. */
		Vec3 position;

		/** UV coordinate. */
		Vec2 uv;

		/**
		 * @brief Description of vertex bindings used by Vulkan.
		 * @note Binding description.
		 */
		static vk::VertexInputBindingDescription get_binding_description();

		/**
		 * @brief Array of descriptions for each vertex input.
		 * @note Vertex input descriptions.
		 */
		static std::array<vk::VertexInputAttributeDescription, 2> get_attribute_descriptions();
	};



	/**
	 * @brief Container of mesh data.
	 */
	class Mesh
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Indices.
		 * @param Vertices.
		 */
		Mesh(Graphics& graphics, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);

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
			return m_index_count;
		}

	private:

		/** Graphics context. */
		Graphics& m_graphics;

		/** Vertex buffer. */
		VkMemBuffer m_vertex_buffer;

		/** Index buffer. */
		VkMemBuffer m_index_buffer;

		/** Number of elements. */
		size_t m_index_count = 0;
	};
}