/**
 * @file shader.cpp
 * @brief Vulkan shader source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "spirv_cross\spirv_glsl.hpp"
#include "material_shader.hpp"
#include "mesh.hpp"

namespace dk
{
	MaterialShader::MaterialShader() {};

	MaterialShader::MaterialShader
	(
		Graphics* graphics,
		std::vector<MaterialShaderCreateInfo>& create_info,
		const std::vector<char>& vert_byte_code,
		const std::vector<char>& frag_byte_code
	)
	{
		m_graphics = graphics;

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
			ShaderPipelineCreateInfo pipeline_create_info = {};

			// Set render pass
			pipeline_create_info.render_pass = create_info[i].render_pass;

			// Vertex shader stage
			if (create_info[i].stage_flags & vk::ShaderStageFlagBits::eVertex)
			{
				vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
				vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
				vert_shader_stage_info.module = m_vk_vertex_shader_module;
				vert_shader_stage_info.pName = "main";
				pipeline_create_info.pipeline_stages.push_back(vert_shader_stage_info);
			}

			// Fragment shader stage
			if (create_info[i].stage_flags & vk::ShaderStageFlagBits::eFragment)
			{
				vk::PipelineShaderStageCreateInfo frag_shader_stage_info = {};
				frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
				frag_shader_stage_info.module = m_vk_fragment_shader_module;
				frag_shader_stage_info.pName = "main";
				pipeline_create_info.pipeline_stages.push_back(frag_shader_stage_info);
			}

			// Set up descriptor set layouts
			pipeline_create_info.descriptor_set_layouts.resize(1 + create_info[i].descriptor_set_layouts.size() + (m_texture_count > 0 ? 1 : 0));
			pipeline_create_info.descriptor_set_layouts[0] = m_vk_descriptor_set_layout;

			for (size_t j = 1; j < 1 + create_info[i].descriptor_set_layouts.size(); ++j)
				pipeline_create_info.descriptor_set_layouts[j] = create_info[i].descriptor_set_layouts[j - 1];

			if (m_texture_count > 0)
				pipeline_create_info.descriptor_set_layouts[pipeline_create_info.descriptor_set_layouts.size() - 1] = m_vk_texture_descriptor_set_layout;

			// Pipeline layout creation
			auto binding_description = Vertex::get_binding_description();
			auto attribute_descriptions = Vertex::get_attribute_descriptions();

			vk::PipelineVertexInputStateCreateInfo& vertex_input_info = pipeline_create_info.vertex_input_info;
			vertex_input_info.vertexBindingDescriptionCount = 1;
			vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
			vertex_input_info.pVertexBindingDescriptions = &binding_description;
			vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

			vk::PipelineInputAssemblyStateCreateInfo& input_assembly = pipeline_create_info.input_assembly;
			input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
			input_assembly.primitiveRestartEnable = VK_FALSE;

			// NOTE : For the viewport and scissor the width and height doesn't really matter
			//		  because the dynamic stage can change them.
			vk::Viewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(m_graphics->get_width());
			viewport.height = static_cast<float>(m_graphics->get_height());
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vk::Rect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = vk::Extent2D(m_graphics->get_width(), m_graphics->get_height());

			vk::PipelineViewportStateCreateInfo& viewport_state = pipeline_create_info.viewport_state;
			viewport_state.viewportCount = 1;
			viewport_state.pViewports = &viewport;
			viewport_state.scissorCount = 1;
			viewport_state.pScissors = &scissor;

			vk::PipelineRasterizationStateCreateInfo& rasterizer = pipeline_create_info.rasterizer;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = vk::PolygonMode::eFill;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = vk::CullModeFlagBits::eBack;
			rasterizer.frontFace = vk::FrontFace::eClockwise;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f;
			rasterizer.depthBiasClamp = 0.0f;
			rasterizer.depthBiasSlopeFactor = 0.0f;

			vk::PipelineMultisampleStateCreateInfo& multisampling = pipeline_create_info.multisampling;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
			multisampling.minSampleShading = 1.0f;
			multisampling.pSampleMask = nullptr;
			multisampling.alphaToCoverageEnable = VK_FALSE;
			multisampling.alphaToOneEnable = VK_FALSE;

			vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
			color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
													vk::ColorComponentFlagBits::eG |
													vk::ColorComponentFlagBits::eB |
													vk::ColorComponentFlagBits::eA;
			color_blend_attachment.blendEnable = VK_FALSE;
			color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne;
			color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero;
			color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
			color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
			color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
			color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;

			vk::PipelineColorBlendStateCreateInfo& color_blending = pipeline_create_info.color_blending;
			color_blending.logicOpEnable = VK_FALSE;
			color_blending.logicOp = vk::LogicOp::eCopy;
			color_blending.attachmentCount = 1;
			color_blending.pAttachments = &color_blend_attachment;
			color_blending.blendConstants[0] = 0.0f;
			color_blending.blendConstants[1] = 0.0f;
			color_blending.blendConstants[2] = 0.0f;
			color_blending.blendConstants[3] = 0.0f;

			std::vector<vk::DynamicState> dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

			vk::PipelineDynamicStateCreateInfo& dynamic_state = pipeline_create_info.dynamic_state;
			dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
			dynamic_state.pDynamicStates = dynamic_states.data();

			vk::PipelineDepthStencilStateCreateInfo& depth_stencil = pipeline_create_info.depth_stencil;
			depth_stencil.depthTestEnable = static_cast<vk::Bool32>(create_info[i].depth_test);
			depth_stencil.depthWriteEnable = static_cast<vk::Bool32>(create_info[i].depth_test) && static_cast<vk::Bool32>(create_info[i].depth_write);
			depth_stencil.depthCompareOp = create_info[i].depth_compare;
			depth_stencil.depthBoundsTestEnable = VK_FALSE;
			depth_stencil.minDepthBounds = 0.0f;
			depth_stencil.maxDepthBounds = 1.0f;
			depth_stencil.stencilTestEnable = VK_FALSE;
			depth_stencil.front = {};
			depth_stencil.back = {};

			m_pipelines[i] = create_shader_pipeline(m_graphics->get_logical_device(), pipeline_create_info);
		}
	}

	void MaterialShader::free()
	{
		Shader::free();
		m_graphics->get_logical_device().destroyDescriptorSetLayout(m_vk_descriptor_set_layout);
		m_graphics->get_logical_device().destroyDescriptorSetLayout(m_vk_texture_descriptor_set_layout);
	}
}