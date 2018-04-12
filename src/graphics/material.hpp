#pragma once

/**
 * @file material.hpp
 * @brief Shader instance.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include "shader.hpp"

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
		 * @brief Get shader.
		 * @return Shader.
		 */
		Handle<Shader> get_shader()
		{
			return m_shader;
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

		/** Vertex uniform buffer. */
		VkMemBuffer m_vertex_uniform_buffer;

		/** Fragment uniform buffer. */
		VkMemBuffer m_fragment_uniform_buffer;
	};
}