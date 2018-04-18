#pragma once

/**
 * @file material.hpp
 * @brief Shader instance.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include "shader.hpp"
#include "texture.hpp"

namespace dk
{
	/**
	 * @brief Shader instance.
	 */
	class Material
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Material();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Shader.
		 */
		Material(Graphics* graphics, Handle<Shader> shader);

		/**
		 * @brief Destructor.
		 */
		~Material() = default;

		/**
		 * @brief Free material resources.
		 */
		void free();

		/**
		 * @brief Set texture.
		 * @param Texture index.
		 * @param Texture handle.
		 */
		void set_texture(size_t index, Handle<Texture> texture);

		/**
		 * @brief Get shader.
		 * @return Shader.
		 */
		Handle<Shader> get_shader()
		{
			return m_shader;
		}

		/**
		 * @brief Get texture.
		 * @param Index.
		 * @return Texture.
		 */
		Handle<Texture> get_texture(size_t index)
		{
			dk_assert(index < m_textures.size());
			return m_textures[index];
		}

		/**
		 * @brief Get vertex uniform buffer.
		 * @return Vertex uniform buffer.
		 */
		const VkMemBuffer& get_vertex_uniform_buffer() const
		{
			return m_vertex_uniform_buffer;
		}

		/**
		 * @brief Get fragment uniform buffer.
		 * @return Fragment uniform buffer.
		 */
		const VkMemBuffer& get_fragment_uniform_buffer() const
		{
			return m_fragment_uniform_buffer;
		}

		/**
		 * @brief Get texture descriptor set.
		 * @return Texture descriptor set.
		 */
		const vk::DescriptorSet& get_texture_descriptor_set() const
		{
			return m_vk_texture_descriptor_set;
		}

		/**
		 * @brief Set vertex data.
		 * @tparam Type of data sent.
		 * @param Vertex data.
		 */
		template<typename T>
		void set_vertex_data(const T& vert_data)
		{
			void* data = m_graphics->get_logical_device().mapMemory(m_vertex_uniform_buffer.memory, 0, sizeof(T));
			memcpy(data, &vert_data, sizeof(T));
			m_graphics->get_logical_device().unmapMemory(m_vertex_uniform_buffer.memory);
		}

		/**
		 * @brief Set fragment data.
		 * @tparam Type of data sent.
		 * @param Fragment data.
		 */
		template<typename T>
		void set_fragment_data(const T& frag_data)
		{
			void* data = m_graphics->get_logical_device().mapMemory(m_fragment_uniform_buffer.memory, 0, sizeof(T));
			memcpy(data, &frag_data, sizeof(T));
			m_graphics->get_logical_device().unmapMemory(m_fragment_uniform_buffer.memory);
		}

	private:

		/** Graphics context. */
		Graphics* m_graphics;

		/** Shader. */
		Handle<Shader> m_shader;

		/** Textures. */
		std::vector<Handle<Texture>> m_textures = {};

		/** Descriptor pool. */
		vk::DescriptorPool m_vk_descriptor_pool = {};

		/** Texture descriptor set. */
		vk::DescriptorSet m_vk_texture_descriptor_set = {};

		/** Vertex uniform buffer. */
		VkMemBuffer m_vertex_uniform_buffer;

		/** Fragment uniform buffer. */
		VkMemBuffer m_fragment_uniform_buffer;
	};
}