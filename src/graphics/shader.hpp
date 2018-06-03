#pragma once

/**
 * @file shader.hpp
 * @brief Vulkan shaders.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <utilities\resource_allocator.hpp>
#include "graphics.hpp"

namespace dk
{
	/**
	 * Shader creation info.
	 */
	struct ShaderCreateInfo
	{
		/** Shader stage flags. */
		vk::ShaderStageFlags stage_flags = {};

		/** Pipeline creation info. */
		ShaderPipelineCreateInfo pipeline_create_info = {};
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
		 * @param Shader create infos.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 * @note If any of the byte code vectors have a size of 0,
		 *       they won't be turned into shader modules.
		 */
		Shader
		(
			Graphics* graphics, 
			std::vector<ShaderCreateInfo>& create_info,
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
		virtual void free();

		/**
		 * @brief Get pipeline.
		 * @param Pipeline index.
		 * @return Pipeline.
		 */
		const ShaderPipeline& get_pipeline(size_t i) const
		{
			dk_assert(i < m_pipelines.size());
			return m_pipelines[i];
		}

		/**
		 * @brief Get the number of textures.
		 * @return Number of textures.
		 */
		size_t get_texture_count() const
		{
			return m_texture_count;
		}

	protected:

		/** Graphics context. */
		Graphics* m_graphics;

		/** Vertex shader module. */
		vk::ShaderModule m_vk_vertex_shader_module;

		/** Fragment shader module. */
		vk::ShaderModule m_vk_fragment_shader_module;

		/** Shader pipelines. */
		std::vector<ShaderPipeline> m_pipelines = {};

		/** Number of textures. */
		size_t m_texture_count;
	};

	/** Handle to a shader. */
	using HShader = Handle<Shader>;
}