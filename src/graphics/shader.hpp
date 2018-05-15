#pragma once

/**
 * @file shader.hpp
 * @brief Vulkan shaders.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include "graphics.hpp"

namespace dk
{
	struct VertexShaderData
	{
		glm::mat4 model;
		glm::mat4 mvp;
	};

	struct FragmentShaderData
	{
		int unused;
	};

	/**
	 * @brief Shader object.
	 */
	class Shader
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Shader();

		/**
		 * @brief Constructor.
		 * @param Graphics context the shader is bound to.
		 * @param Render pass used by the shader.
		 * @param Render pass for depth only rendering.
		 * @param Custom descriptor set layouts.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 * @param Is depth testing enabled?
		 */
		Shader
		(
			Graphics* graphics, 
			const vk::RenderPass& render_pass,
			const vk::RenderPass& depth_pass,
			const std::vector<vk::DescriptorSetLayout>& dsl,
			const std::vector<char>& vert_byte_code, 
			const std::vector<char>& frag_byte_code,
			bool depth_testing = true
		);

		/**
		 * @brief Destructor.
		 */
		~Shader() = default;

		/**
		 * @brief Free memory used by the shader.
		 * @note Used internally. Do not call.
		 */
		void free();

		/**
		 * @brief Get graphics pipeline.
		 * @return Graphics pipeline.
		 */
		vk::Pipeline& get_graphics_pipeline()
		{
			return m_vk_graphics_pipeline;
		}

		/**
		 * @brief Get graphics pipeline layout.
		 * @return Graphics pipeline layout.
		 */
		vk::PipelineLayout& get_graphics_pipeline_layout()
		{
			return m_vk_graphics_pipeline_layout;
		}

		/**
		 * @brief Get depth pipeline.
		 * @return Depth pipeline.
		 */
		vk::Pipeline& get_depth_pipeline()
		{
			return m_vk_depth_pipeline;
		}

		/**
		 * @brief Get depth pipeline layout.
		 * @return Depth pipeline layout.
		 */
		vk::PipelineLayout& get_depth_pipeline_layout()
		{
			return m_vk_depth_pipeline_layout;
		}

		/**
		 * @brief Get the number of textures.
		 * @return Number of textures.
		 */
		size_t get_texture_count() const
		{
			return m_texture_count;
		}

		/**
		 * @brief Get size in bytes of the vertex uniform buffer.
		 * @return Size of vertex uniform buffer.
		 */
		size_t get_vertex_buffer_size() const
		{
			return m_vertex_buffer_size;
		}

		/**
		 * @brief Get size in bytes of the per instance vertex uniform buffer.
		 * @return Size of the per instance vertex uniform buffer.
		 */
		size_t get_inst_vertex_buffer_size() const
		{
			return m_inst_vertex_buffer_size;
		}

		/**
		 * @brief Get size in bytes of the fragment uniform buffer.
		 * @return Size of fragment uniform buffer.
		 */
		size_t get_fragment_buffer_size() const
		{
			return m_fragment_buffer_size;
		}

		/**
		 * @brief Get size in bytes of the per instance fragment uniform buffer.
		 * @return Size of the per instance fragment uniform buffer.
		 */
		size_t get_inst_fragment_buffer_size() const
		{
			return m_inst_fragment_buffer_size;
		}

		/**
		 * @brief Get descriptor set layout.
		 * @return Descriptor set layout.
		 */
		vk::DescriptorSetLayout& get_descriptor_set_layout()
		{
			return m_vk_descriptor_set_layout;
		}

		/**
		 * @brief Get texture descriptor set layout.
		 * @return Texture descriptor set layout.
		 */
		vk::DescriptorSetLayout& get_texture_descriptor_set_layout()
		{
			return m_vk_texture_descriptor_set_layout;
		}

	private:

		/** Graphics context. */
		Graphics* m_graphics;

		/** Vertex shader module. */
		vk::ShaderModule m_vk_vertex_shader_module;

		/** Fragment shader module. */
		vk::ShaderModule m_vk_fragment_shader_module;

		/** Descriptor set layout. */
		vk::DescriptorSetLayout m_vk_descriptor_set_layout;

		/** Texture descriptor set layout. */
		vk::DescriptorSetLayout m_vk_texture_descriptor_set_layout;

		/** Graphics pipeline layout */
		vk::PipelineLayout m_vk_graphics_pipeline_layout;

		/** Graphics pipeline. */
		vk::Pipeline m_vk_graphics_pipeline;

		/** Depth pipeline layout */
		vk::PipelineLayout m_vk_depth_pipeline_layout;

		/** Depth pipeline. */
		vk::Pipeline m_vk_depth_pipeline;

		/** Number of textures. */
		size_t m_texture_count;

		/** Size in bytes of the vertex uniform buffer. */
		size_t m_vertex_buffer_size;

		/** Size in bytes of the per instance vertex uniform buffer. */
		size_t m_inst_vertex_buffer_size;

		/** Size in bytes of the fragment uniform buffer. */
		size_t m_fragment_buffer_size;

		/** Size in bytes of the per instance fragment uniform buffer. */
		size_t m_inst_fragment_buffer_size;
	};
}