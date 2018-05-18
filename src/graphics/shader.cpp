/**
 * @file shader.cpp
 * @brief Vulkan shader source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "spirv_cross\spirv_glsl.hpp"
#include "shader.hpp"
#include "mesh.hpp"

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
			// Analyze vertex shader byte code
			size_t word_count = vert_byte_code.size() / 4;
			dk_assert((word_count * 4) == vert_byte_code.size());
			spirv_cross::CompilerGLSL glsl(reinterpret_cast<const uint32_t*>(vert_byte_code.data()), word_count);

			// Find sizes for uniform buffers
			spirv_cross::ShaderResources resources = glsl.get_shader_resources();
			for (const auto& buffer : resources.uniform_buffers)
			{
				const spirv_cross::SPIRType& type = glsl.get_type(buffer.base_type_id);
				size_t size = glsl.get_declared_struct_size(type);

				if (buffer.name == "MaterialData")
					m_vertex_buffer_size = size;
				else
					m_inst_vertex_buffer_size = size;
			}
		}

		{
			// Analyze fragment shader byte code
			size_t word_count = frag_byte_code.size() / 4;
			dk_assert((word_count * 4) == frag_byte_code.size());
			spirv_cross::CompilerGLSL glsl(reinterpret_cast<const uint32_t*>(frag_byte_code.data()), word_count);
			
			// Find sizes for uniform buffers
			spirv_cross::ShaderResources resources = glsl.get_shader_resources();
			for (const auto& buffer : resources.uniform_buffers)
			{
				const spirv_cross::SPIRType& type = glsl.get_type(buffer.base_type_id);
				size_t size = glsl.get_declared_struct_size(type);

				if (buffer.name == "MaterialData")
					m_fragment_buffer_size = size;
				else
					m_inst_fragment_buffer_size = size;
			}

			// Get texture count
			m_texture_count = resources.sampled_images.size();
		}

		dk_assert
		(
			m_fragment_buffer_size > 0 &&
			m_inst_fragment_buffer_size > 0 &&
			m_vertex_buffer_size > 0 &&
			m_inst_vertex_buffer_size > 0
		);

		// Create descriptor set layout
		{
			std::array<vk::DescriptorSetLayoutBinding, 4> bindings = {};

			bindings[0].binding = 0;
			bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			bindings[0].descriptorCount = 1;
			bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;

			bindings[1].binding = 1;
			bindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
			bindings[1].descriptorCount = 1;
			bindings[1].stageFlags = vk::ShaderStageFlagBits::eVertex;

			bindings[2].binding = 2;
			bindings[2].descriptorType = vk::DescriptorType::eUniformBuffer;
			bindings[2].descriptorCount = 1;
			bindings[2].stageFlags = vk::ShaderStageFlagBits::eFragment;

			bindings[3].binding = 3;
			bindings[3].descriptorType = vk::DescriptorType::eUniformBuffer;
			bindings[3].descriptorCount = 1;
			bindings[3].stageFlags = vk::ShaderStageFlagBits::eFragment;

			vk::DescriptorSetLayoutCreateInfo layout_info = {};
			layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_info.pBindings = bindings.data();

			m_vk_descriptor_set_layout = m_graphics->get_logical_device().createDescriptorSetLayout(layout_info);
			dk_assert(m_vk_descriptor_set_layout);
		}

		// Create texture descriptor set layout
		if(m_texture_count > 0)
		{
			std::vector<vk::DescriptorSetLayoutBinding> bindings(m_texture_count);

			for (size_t i = 0; i < m_texture_count; ++i)
			{
				bindings[i].binding = static_cast<uint32_t>(i);
				bindings[i].descriptorType = vk::DescriptorType::eCombinedImageSampler;
				bindings[i].descriptorCount = 1;
				bindings[i].stageFlags = vk::ShaderStageFlagBits::eFragment;
			}

			vk::DescriptorSetLayoutCreateInfo layout_info = {};
			layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_info.pBindings = bindings.data();

			m_vk_texture_descriptor_set_layout = m_graphics->get_logical_device().createDescriptorSetLayout(layout_info);
			dk_assert(m_vk_texture_descriptor_set_layout);
		}

		// Create shader modules
		m_vk_vertex_shader_module = create_shader_module(m_graphics->get_logical_device(), vert_byte_code);
		m_vk_fragment_shader_module = create_shader_module(m_graphics->get_logical_device(), frag_byte_code);

		// Resize pipeline vector
		m_pipelines = std::vector<ShaderPipeline>(create_info.size());

		// Create pipelines
		for (size_t i = 0; i < create_info.size(); ++i)
		{
			std::vector<vk::PipelineShaderStageCreateInfo> stage_info = {};

			if (create_info[i].stage_flags & vk::ShaderStageFlagBits::eVertex)
			{
				vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
				vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
				vert_shader_stage_info.module = m_vk_vertex_shader_module;
				vert_shader_stage_info.pName = "main";
				stage_info.push_back(vert_shader_stage_info);
			}

			if (create_info[i].stage_flags & vk::ShaderStageFlagBits::eFragment)
			{
				vk::PipelineShaderStageCreateInfo frag_shader_stage_info = {};
				frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
				frag_shader_stage_info.module = m_vk_fragment_shader_module;
				frag_shader_stage_info.pName = "main";
				stage_info.push_back(frag_shader_stage_info);
			}

			std::vector<vk::DescriptorSetLayout> descriptor_set_layouts(1 + create_info[i].descriptor_set_layouts.size() + (m_texture_count > 0 ? 1 : 0));
			descriptor_set_layouts[0] = m_vk_descriptor_set_layout;

			for (size_t j = 1; j < 1 + create_info[i].descriptor_set_layouts.size(); ++j)
				descriptor_set_layouts[j] = create_info[i].descriptor_set_layouts[j - 1];

			if (m_texture_count > 0)
				descriptor_set_layouts[descriptor_set_layouts.size() - 1] = m_vk_texture_descriptor_set_layout;

			ShaderPipelineCreateInfo pipeline_create_info = {};
			pipeline_create_info.pipeline_stages = stage_info;
			pipeline_create_info.width = static_cast<uint32_t>(m_graphics->get_width());
			pipeline_create_info.height = static_cast<uint32_t>(m_graphics->get_height());
			pipeline_create_info.render_pass = create_info[i].render_pass;
			pipeline_create_info.dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
			pipeline_create_info.depth_test = create_info[i].depth_test;
			pipeline_create_info.depth_compare = create_info[i].depth_compare;
			pipeline_create_info.depth_write = create_info[i].depth_write;
			pipeline_create_info.descriptor_set_layouts = descriptor_set_layouts;
			
			m_pipelines[i] = create_shader_pipeline(m_graphics->get_logical_device(), pipeline_create_info);
		}
	}

	void Shader::free()
	{
		for (auto& pipeline : m_pipelines)
		{
			m_graphics->get_logical_device().destroyPipeline(pipeline.pipeline);
			m_graphics->get_logical_device().destroyPipelineLayout(pipeline.layout);
		}

		m_graphics->get_logical_device().destroyShaderModule(m_vk_vertex_shader_module);
		m_graphics->get_logical_device().destroyShaderModule(m_vk_fragment_shader_module);
		m_graphics->get_logical_device().destroyDescriptorSetLayout(m_vk_descriptor_set_layout);
		m_graphics->get_logical_device().destroyDescriptorSetLayout(m_vk_texture_descriptor_set_layout);
	}
}