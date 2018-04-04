/**
 * @file shader.cpp
 * @brief Vulkan shader source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "shader.hpp"

namespace dk
{
	Shader::Shader
	(
		Graphics& graphics,
		const vk::RenderPass& render_pass,
		const std::vector<char>& vert_byte_code,
		const std::vector<char>& frag_byte_code
	) : m_graphics(graphics)
	{
		// Create shader modules
		m_vk_vertex_shader_module = create_shader_module(m_graphics.get_logical_device(), vert_byte_code);
		m_vk_fragment_shader_module = create_shader_module(m_graphics.get_logical_device(), frag_byte_code);

		vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
		vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
		vert_shader_stage_info.module = m_vk_vertex_shader_module;
		vert_shader_stage_info.pName = "main";

		vk::PipelineShaderStageCreateInfo frag_shader_stage_info = {};
		frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
		frag_shader_stage_info.module = m_vk_fragment_shader_module;
		frag_shader_stage_info.pName = "main";

		std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages = { vert_shader_stage_info, frag_shader_stage_info };

		vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.vertexBindingDescriptionCount = 0;
		vertex_input_info.pVertexBindingDescriptions = nullptr;
		vertex_input_info.vertexAttributeDescriptionCount = 0;
		vertex_input_info.pVertexAttributeDescriptions = nullptr;

		vk::PipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(graphics.get_width());
		viewport.height = static_cast<float>(graphics.get_height());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = vk::Extent2D(graphics.get_width(), graphics.get_height());

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

		// Create graphics pipeline layout
		vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pSetLayouts = nullptr;
		pipeline_layout_info.pushConstantRangeCount = 0;
		pipeline_layout_info.pPushConstantRanges = nullptr;

		m_vk_pipeline_layout = m_graphics.get_logical_device().createPipelineLayout(pipeline_layout_info);
		dk_assert(m_vk_pipeline_layout);

		// Create graphics pipeline
		vk::GraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
		pipeline_info.pStages = shader_stages.data();
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = nullptr;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = m_vk_pipeline_layout;
		pipeline_info.renderPass = render_pass;
		pipeline_info.subpass = 0;

		m_vk_graphics_pipeline = m_graphics.get_logical_device().createGraphicsPipeline(vk::PipelineCache(), pipeline_info);
	}

	void Shader::free()
	{
		m_graphics.get_logical_device().destroyPipeline(m_vk_graphics_pipeline);
		m_graphics.get_logical_device().destroyPipelineLayout(m_vk_pipeline_layout);
		m_graphics.get_logical_device().destroyShaderModule(m_vk_vertex_shader_module);
		m_graphics.get_logical_device().destroyShaderModule(m_vk_fragment_shader_module);
	}
}