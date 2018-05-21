/**
 * @file shader.cpp
 * @brief Vulkan shader source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "spirv_cross\spirv_glsl.hpp"
#include "shader.hpp"

namespace dk
{
	Shader::Shader() {};

	Shader::Shader
	(
		Graphics* graphics,
		std::vector<ShaderCreateInfo>& create_info,
		const std::vector<char>& vert_byte_code,
		const std::vector<char>& frag_byte_code
	) : m_graphics(graphics)
	{
		{
			// Analyze fragment shader byte code
			size_t word_count = frag_byte_code.size() / 4;
			dk_assert((word_count * 4) == frag_byte_code.size());
			spirv_cross::CompilerGLSL glsl(reinterpret_cast<const uint32_t*>(frag_byte_code.data()), word_count);
			
			// Find sizes for uniform buffers
			spirv_cross::ShaderResources resources = glsl.get_shader_resources();

			// Get texture count
			m_texture_count = resources.sampled_images.size();
		}

		// Create shader modules
		if (vert_byte_code.size() > 0)
		{
			m_vk_vertex_shader_module = create_shader_module(m_graphics->get_logical_device(), vert_byte_code);
			dk_assert(m_vk_vertex_shader_module);
		}

		if (frag_byte_code.size() > 0)
		{
			m_vk_fragment_shader_module = create_shader_module(m_graphics->get_logical_device(), frag_byte_code);
			dk_assert(m_vk_fragment_shader_module);
		}

		// Fill in pipelines
		for (auto& info : create_info)
		{
			if (info.stage_flags & vk::ShaderStageFlagBits::eVertex)
			{
				dk_assert(m_vk_vertex_shader_module);
				vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
				vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
				vert_shader_stage_info.module = m_vk_vertex_shader_module;
				vert_shader_stage_info.pName = "main";
				info.pipeline_create_info.pipeline_stages.push_back(vert_shader_stage_info);
			}

			if (info.stage_flags & vk::ShaderStageFlagBits::eFragment)
			{
				dk_assert(m_vk_fragment_shader_module);
				vk::PipelineShaderStageCreateInfo frag_shader_stage_info = {};
				frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
				frag_shader_stage_info.module = m_vk_fragment_shader_module;
				frag_shader_stage_info.pName = "main";
				info.pipeline_create_info.pipeline_stages.push_back(frag_shader_stage_info);
			}
		}

		// Resize pipeline vector
		m_pipelines = std::vector<ShaderPipeline>(create_info.size());

		// Create pipelines
		for (size_t i = 0; i < create_info.size(); ++i)
		{
			m_pipelines[i] = create_shader_pipeline(m_graphics->get_logical_device(), create_info[i].pipeline_create_info);
		}
	}

	void Shader::free()
	{
		for (auto& pipeline : m_pipelines)
		{
			m_graphics->get_logical_device().destroyPipeline(pipeline.pipeline);
			m_graphics->get_logical_device().destroyPipelineLayout(pipeline.layout);
		}

		if(m_vk_vertex_shader_module) m_graphics->get_logical_device().destroyShaderModule(m_vk_vertex_shader_module);
		if(m_vk_fragment_shader_module) m_graphics->get_logical_device().destroyShaderModule(m_vk_fragment_shader_module);
	}
}