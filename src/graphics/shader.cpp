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
		const vk::RenderPass& render_pass,
		const vk::RenderPass& depth_pass,
		const std::vector<vk::DescriptorSetLayout>& dsl,
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

		// Depth pipeline
		{
			vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
			vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
			vert_shader_stage_info.module = m_vk_vertex_shader_module;
			vert_shader_stage_info.pName = "main";

			auto binding_description = Vertex::get_binding_description();
			auto attribute_descriptions = Vertex::get_attribute_descriptions();

			vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
			vertex_input_info.vertexBindingDescriptionCount = 1;
			vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
			vertex_input_info.pVertexBindingDescriptions = &binding_description;
			vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

			vk::PipelineInputAssemblyStateCreateInfo input_assembly = {};
			input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
			input_assembly.primitiveRestartEnable = VK_FALSE;

			vk::Viewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(graphics->get_width());
			viewport.height = static_cast<float>(graphics->get_height());
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vk::Rect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = vk::Extent2D(graphics->get_width(), graphics->get_height());

			vk::PipelineViewportStateCreateInfo viewport_state = {};
			viewport_state.viewportCount = 1;
			viewport_state.pViewports = &viewport;
			viewport_state.scissorCount = 1;
			viewport_state.pScissors = &scissor;

			vk::PipelineRasterizationStateCreateInfo rasterizer = {};
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

			vk::PipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
			multisampling.minSampleShading = 1.0f;
			multisampling.pSampleMask = nullptr;
			multisampling.alphaToCoverageEnable = VK_FALSE;
			multisampling.alphaToOneEnable = VK_FALSE;

			std::array<vk::DynamicState, 2> dynamic_states =
			{
				vk::DynamicState::eViewport,
				vk::DynamicState::eScissor
			};

			vk::PipelineDynamicStateCreateInfo dynamic_state = {};
			dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
			dynamic_state.pDynamicStates = dynamic_states.data();

			vk::PipelineDepthStencilStateCreateInfo depth_stencil = {};
			depth_stencil.depthTestEnable = VK_TRUE;
			depth_stencil.depthWriteEnable = VK_TRUE;
			depth_stencil.depthCompareOp = vk::CompareOp::eLess;
			depth_stencil.depthBoundsTestEnable = VK_FALSE;
			depth_stencil.minDepthBounds = 0.0f;
			depth_stencil.maxDepthBounds = 1.0f;
			depth_stencil.stencilTestEnable = VK_FALSE;
			depth_stencil.front = {};
			depth_stencil.back = {};

			std::vector<vk::DescriptorSetLayout> descriptor_set_layouts(1 + dsl.size() + (m_texture_count > 0 ? 1 : 0));
			descriptor_set_layouts[0] = m_vk_descriptor_set_layout;

			for (size_t i = 1; i < 1 + dsl.size(); ++i)
				descriptor_set_layouts[i] = dsl[i - 1];

			if (m_texture_count > 0)
				descriptor_set_layouts[descriptor_set_layouts.size() - 1] = m_vk_texture_descriptor_set_layout;

			// Create depth pipeline layout
			vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
			pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
			pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
			pipeline_layout_info.pushConstantRangeCount = 0;
			pipeline_layout_info.pPushConstantRanges = nullptr;

			m_vk_depth_pipeline_layout = m_graphics->get_logical_device().createPipelineLayout(pipeline_layout_info);
			dk_assert(m_vk_depth_pipeline_layout);

			// Create depth pipeline
			vk::GraphicsPipelineCreateInfo pipeline_info = {};
			pipeline_info.stageCount = 1;
			pipeline_info.pStages = &vert_shader_stage_info;
			pipeline_info.pVertexInputState = &vertex_input_info;
			pipeline_info.pInputAssemblyState = &input_assembly;
			pipeline_info.pViewportState = &viewport_state;
			pipeline_info.pRasterizationState = &rasterizer;
			pipeline_info.pMultisampleState = &multisampling;
			pipeline_info.pDepthStencilState = &depth_stencil;
			pipeline_info.pColorBlendState = nullptr;
			pipeline_info.pDynamicState = &dynamic_state;
			pipeline_info.layout = m_vk_depth_pipeline_layout;
			pipeline_info.renderPass = depth_pass;
			pipeline_info.subpass = 0;

			m_vk_depth_pipeline = m_graphics->get_logical_device().createGraphicsPipeline(vk::PipelineCache(), pipeline_info);
			dk_assert(m_vk_depth_pipeline);
		}

		// Rendering pipeline
		{
			vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
			vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
			vert_shader_stage_info.module = m_vk_vertex_shader_module;
			vert_shader_stage_info.pName = "main";

			vk::PipelineShaderStageCreateInfo frag_shader_stage_info = {};
			frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
			frag_shader_stage_info.module = m_vk_fragment_shader_module;
			frag_shader_stage_info.pName = "main";

			std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages = { vert_shader_stage_info, frag_shader_stage_info };

			auto binding_description = Vertex::get_binding_description();
			auto attribute_descriptions = Vertex::get_attribute_descriptions();

			vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
			vertex_input_info.vertexBindingDescriptionCount = 1;
			vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
			vertex_input_info.pVertexBindingDescriptions = &binding_description;
			vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

			vk::PipelineInputAssemblyStateCreateInfo input_assembly = {};
			input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
			input_assembly.primitiveRestartEnable = VK_FALSE;

			vk::Viewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(graphics->get_width());
			viewport.height = static_cast<float>(graphics->get_height());
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vk::Rect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = vk::Extent2D(graphics->get_width(), graphics->get_height());

			vk::PipelineViewportStateCreateInfo viewport_state = {};
			viewport_state.viewportCount = 1;
			viewport_state.pViewports = &viewport;
			viewport_state.scissorCount = 1;
			viewport_state.pScissors = &scissor;

			vk::PipelineRasterizationStateCreateInfo rasterizer = {};
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

			vk::PipelineMultisampleStateCreateInfo multisampling = {};
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

			vk::PipelineColorBlendStateCreateInfo color_blending = {};
			color_blending.logicOpEnable = VK_FALSE;
			color_blending.logicOp = vk::LogicOp::eCopy;
			color_blending.attachmentCount = 1;
			color_blending.pAttachments = &color_blend_attachment;
			color_blending.blendConstants[0] = 0.0f;
			color_blending.blendConstants[1] = 0.0f;
			color_blending.blendConstants[2] = 0.0f;
			color_blending.blendConstants[3] = 0.0f;

			std::array<vk::DynamicState, 2> dynamic_states =
			{
				vk::DynamicState::eViewport,
				vk::DynamicState::eScissor
			};

			vk::PipelineDynamicStateCreateInfo dynamic_state = {};
			dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
			dynamic_state.pDynamicStates = dynamic_states.data();

			vk::PipelineDepthStencilStateCreateInfo depth_stencil = {};
			depth_stencil.depthTestEnable = VK_TRUE;
			depth_stencil.depthWriteEnable = VK_FALSE;
			depth_stencil.depthCompareOp = vk::CompareOp::eEqual;
			depth_stencil.depthBoundsTestEnable = VK_FALSE;
			depth_stencil.minDepthBounds = 0.0f;
			depth_stencil.maxDepthBounds = 1.0f;
			depth_stencil.stencilTestEnable = VK_FALSE;
			depth_stencil.front = {};
			depth_stencil.back = {};

			std::vector<vk::DescriptorSetLayout> descriptor_set_layouts(1 + dsl.size() + (m_texture_count > 0 ? 1 : 0));
			descriptor_set_layouts[0] = m_vk_descriptor_set_layout;

			for (size_t i = 1; i < 1 + dsl.size(); ++i)
				descriptor_set_layouts[i] = dsl[i - 1];

			if (m_texture_count > 0)
				descriptor_set_layouts[descriptor_set_layouts.size() - 1] = m_vk_texture_descriptor_set_layout;

			// Create graphics pipeline layout
			vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
			pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
			pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
			pipeline_layout_info.pushConstantRangeCount = 0;
			pipeline_layout_info.pPushConstantRanges = nullptr;

			m_vk_graphics_pipeline_layout = m_graphics->get_logical_device().createPipelineLayout(pipeline_layout_info);
			dk_assert(m_vk_graphics_pipeline_layout);

			// Create graphics pipeline
			vk::GraphicsPipelineCreateInfo pipeline_info = {};
			pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
			pipeline_info.pStages = shader_stages.data();
			pipeline_info.pVertexInputState = &vertex_input_info;
			pipeline_info.pInputAssemblyState = &input_assembly;
			pipeline_info.pViewportState = &viewport_state;
			pipeline_info.pRasterizationState = &rasterizer;
			pipeline_info.pMultisampleState = &multisampling;
			pipeline_info.pDepthStencilState = &depth_stencil;
			pipeline_info.pColorBlendState = &color_blending;
			pipeline_info.pDynamicState = &dynamic_state;
			pipeline_info.layout = m_vk_graphics_pipeline_layout;
			pipeline_info.renderPass = render_pass;
			pipeline_info.subpass = 0;

			m_vk_graphics_pipeline = m_graphics->get_logical_device().createGraphicsPipeline(vk::PipelineCache(), pipeline_info);
			dk_assert(m_vk_graphics_pipeline);
		}
	}

	void Shader::free()
	{
		m_graphics->get_logical_device().destroyPipeline(m_vk_depth_pipeline);
		m_graphics->get_logical_device().destroyPipelineLayout(m_vk_depth_pipeline_layout);
		m_graphics->get_logical_device().destroyPipeline(m_vk_graphics_pipeline);
		m_graphics->get_logical_device().destroyPipelineLayout(m_vk_graphics_pipeline_layout);
		m_graphics->get_logical_device().destroyShaderModule(m_vk_vertex_shader_module);
		m_graphics->get_logical_device().destroyShaderModule(m_vk_fragment_shader_module);
		m_graphics->get_logical_device().destroyDescriptorSetLayout(m_vk_descriptor_set_layout);
		m_graphics->get_logical_device().destroyDescriptorSetLayout(m_vk_texture_descriptor_set_layout);
	}
}