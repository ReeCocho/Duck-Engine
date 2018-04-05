#pragma once

/**
 * @file shader.hpp
 * @brief Vulkan shaders.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "graphics.hpp"

namespace dk
{
	/**
	 * @brief Shader object.
	 */
	class Shader
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Graphics context the shader is bound to.
		 * @param Render pass used by the shader.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 */
		Shader
		(
			Graphics& graphics, 
			const vk::RenderPass& render_pass,
			const std::vector<char>& vert_byte_code, 
			const std::vector<char>& frag_byte_code
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

	private:

		/** Graphics context. */
		Graphics& m_graphics;

		/** Vertex shader module. */
		vk::ShaderModule m_vk_vertex_shader_module;

		/** Fragment shader module. */
		vk::ShaderModule m_vk_fragment_shader_module;

		/** Graphics pipeline layout */
		vk::PipelineLayout m_vk_pipeline_layout;

		/** Graphics pipeline. */
		vk::Pipeline m_vk_graphics_pipeline;
	};
}