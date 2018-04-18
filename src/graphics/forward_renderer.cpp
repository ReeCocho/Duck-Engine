/**
 * @file forward_renderer.cpp
 * @brief Duck forward renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <array>
#include <utilities\file_io.hpp>
#include "forward_renderer.hpp"

namespace
{
	/** Realative path to forward renderer vertex shader. */
	const std::string FORWARD_RENDERER_VERTEX_SHADER = "shaders/forward.vert.spv";

	/** Realative path to forward renderer fragment shader. */
	const std::string FORWARD_RENDERER_FRAGMENT_SHADER = "shaders/forward.frag.spv";
}

namespace dk
{
	ForwardRenderer::ForwardRenderer() : Renderer()
	{

	}

	ForwardRenderer::ForwardRenderer(Graphics* graphics, ResourceAllocator<Texture>* texture_allocator, ResourceAllocator<Mesh>* mesh_allocator) : 
		Renderer(graphics, texture_allocator, mesh_allocator)
	{
		// Create on screen pass
		{
			// Describes color attachment usage
			vk::AttachmentDescription color_attachment = {};
			color_attachment.format = get_swapchain_manager().get_image_format();
			color_attachment.samples = vk::SampleCountFlagBits::e1;
			color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
			color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
			color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			color_attachment.initialLayout = vk::ImageLayout::eUndefined;
			color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

			vk::AttachmentReference color_attachment_ref = {};
			color_attachment_ref.attachment = 0;
			color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &color_attachment_ref;

			vk::SubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
			dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

			vk::RenderPassCreateInfo render_pass_info = {};
			render_pass_info.attachmentCount = 1;
			render_pass_info.pAttachments = &color_attachment;
			render_pass_info.subpassCount = 1;
			render_pass_info.pSubpasses = &subpass;
			render_pass_info.dependencyCount = 1;
			render_pass_info.pDependencies = &dependency;

			m_vk_on_screen_pass = get_graphics().get_logical_device().createRenderPass(render_pass_info);
		}

		// Create shader pass
		{
			std::array<vk::AttachmentDescription, 2> attachment_descs = {};

			attachment_descs[0].format = get_swapchain_manager().get_image_format();
			attachment_descs[0].samples = vk::SampleCountFlagBits::e1;
			attachment_descs[0].loadOp = vk::AttachmentLoadOp::eClear;
			attachment_descs[0].storeOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[0].stencilLoadOp = vk::AttachmentLoadOp::eClear;
			attachment_descs[0].stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[0].initialLayout = vk::ImageLayout::eUndefined;
			attachment_descs[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

			attachment_descs[1].format = find_best_depth_format(get_graphics().get_physical_device());
			attachment_descs[1].samples = vk::SampleCountFlagBits::e1;
			attachment_descs[1].loadOp = vk::AttachmentLoadOp::eClear;
			attachment_descs[1].storeOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
			attachment_descs[1].stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[1].initialLayout = vk::ImageLayout::eUndefined;
			attachment_descs[1].finalLayout = vk::ImageLayout::ePresentSrcKHR;

			std::array<vk::AttachmentReference, 2> attachment_refs = {};
			attachment_refs[0].attachment = 0;
			attachment_refs[0].layout = vk::ImageLayout::eColorAttachmentOptimal;

			attachment_refs[1].attachment = 1;
			attachment_refs[1].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &attachment_refs[0];
			subpass.pDepthStencilAttachment = &attachment_refs[1];

			// Use subpass dependencies for attachment layput transitions
			std::array<vk::SubpassDependency, 2> dependencies =
			{
				vk::SubpassDependency
				(
					VK_SUBPASS_EXTERNAL,
					0,
					vk::PipelineStageFlagBits::eBottomOfPipe,
					vk::PipelineStageFlagBits::eColorAttachmentOutput,
					vk::AccessFlagBits::eMemoryRead,
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
					vk::DependencyFlagBits::eByRegion
				),
				vk::SubpassDependency
				(
					0,
					VK_SUBPASS_EXTERNAL,
					vk::PipelineStageFlagBits::eColorAttachmentOutput,
					vk::PipelineStageFlagBits::eBottomOfPipe,
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
					vk::AccessFlagBits::eMemoryRead,
					vk::DependencyFlagBits::eByRegion
				)
			};

			// Create render pass
			vk::RenderPassCreateInfo render_pass_info = {};
			render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_descs.size());
			render_pass_info.pAttachments = attachment_descs.data();
			render_pass_info.subpassCount = 1;
			render_pass_info.pSubpasses = &subpass;
			render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
			render_pass_info.pDependencies = dependencies.data();

			m_vk_shader_pass = get_graphics().get_logical_device().createRenderPass(render_pass_info);
		}

		// Create framebuffers
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++) 
		{
			auto attachment = get_swapchain_manager().get_image_view(i);

			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_vk_on_screen_pass;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = &attachment;
			framebuffer_info.width = get_swapchain_manager().get_image_extent().width;
			framebuffer_info.height = get_swapchain_manager().get_image_extent().height;
			framebuffer_info.layers = 1;

			m_vk_framebuffers[i] = get_graphics().get_logical_device().createFramebuffer(framebuffer_info);
			dk_assert(m_vk_framebuffers[i]);
		}

		// Create thread pool
		m_thread_pool = std::make_unique<ThreadPool>(get_graphics().get_command_manager().get_pool_count());

		// Create quad
		m_quad = Handle<Mesh>(m_mesh_allocator->allocate(), m_mesh_allocator);

		::new(m_mesh_allocator->get_resource_by_handle(m_quad.id))(Mesh)
		(
			&get_graphics(),
			std::vector<uint16_t>
			{
				0, 1, 2,
				2, 3, 0
			},
			std::vector<dk::Vertex>
			{
				{ glm::vec3(-1, -1,  0), glm::vec2(1, 1) },
				{ glm::vec3( 1, -1,  0), glm::vec2(0, 1) },
				{ glm::vec3( 1,  1,  0), glm::vec2(0, 0) },
				{ glm::vec3(-1,  1,  0), glm::vec2(1, 0) }
			}
		);

		// Create screen shader
		{
			// Create descriptor set layout
			vk::DescriptorSetLayoutBinding binding = {};

			binding.binding = 0;
			binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			binding.descriptorCount = 1;
			binding.stageFlags = vk::ShaderStageFlagBits::eFragment;

			vk::DescriptorSetLayoutCreateInfo layout_info = {};
			layout_info.bindingCount = 1;
			layout_info.pBindings = &binding;

			m_screen_shader.vk_descriptor_set_layout = get_graphics().get_logical_device().createDescriptorSetLayout(layout_info);
			dk_assert(m_screen_shader.vk_descriptor_set_layout);

			// Create shader modules
			m_screen_shader.vk_vertex_shader_module = create_shader_module(get_graphics().get_logical_device(), read_binary_file(FORWARD_RENDERER_VERTEX_SHADER));
			m_screen_shader.vk_fragment_shader_module = create_shader_module(get_graphics().get_logical_device(), read_binary_file(FORWARD_RENDERER_FRAGMENT_SHADER));

			vk::PipelineShaderStageCreateInfo vert_shader_stage_info = {};
			vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
			vert_shader_stage_info.module = m_screen_shader.vk_vertex_shader_module;
			vert_shader_stage_info.pName = "main";

			vk::PipelineShaderStageCreateInfo frag_shader_stage_info = {};
			frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
			frag_shader_stage_info.module = m_screen_shader.vk_fragment_shader_module;
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

			// Create graphics pipeline layout
			vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
			pipeline_layout_info.setLayoutCount = 1;
			pipeline_layout_info.pSetLayouts = &m_screen_shader.vk_descriptor_set_layout;
			pipeline_layout_info.pushConstantRangeCount = 0;
			pipeline_layout_info.pPushConstantRanges = nullptr;

			m_screen_shader.vk_pipeline_layout = get_graphics().get_logical_device().createPipelineLayout(pipeline_layout_info);
			dk_assert(m_screen_shader.vk_pipeline_layout);

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
			pipeline_info.layout = m_screen_shader.vk_pipeline_layout;
			pipeline_info.renderPass = m_vk_on_screen_pass;
			pipeline_info.subpass = 0;

			m_screen_shader.vk_graphics_pipeline = get_graphics().get_logical_device().createGraphicsPipeline(vk::PipelineCache(), pipeline_info);

			vk::DescriptorPoolSize pool_size = {};
			pool_size.type = vk::DescriptorType::eCombinedImageSampler;
			pool_size.descriptorCount = 1;

			// Create descriptor pool
			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			pool_info.maxSets = 1;

			m_screen_shader.vk_descriptor_pool = get_graphics().get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_screen_shader.vk_descriptor_pool);

			// Allocate descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_screen_shader.vk_descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_screen_shader.vk_descriptor_set_layout;

			m_screen_shader.vk_descriptor_set = get_graphics().get_logical_device().allocateDescriptorSets(alloc_info)[0];
			dk_assert(m_screen_shader.vk_descriptor_set);
		}
	}

	void ForwardRenderer::shutdown()
	{
		// Destroy thread pool
		m_thread_pool->wait();
		m_thread_pool.reset();

		// Destroy quad
		m_quad->free();
		m_mesh_allocator->deallocate(m_quad.id);

		// Destroy screen shader
		get_graphics().get_logical_device().destroyDescriptorPool(m_screen_shader.vk_descriptor_pool);
		get_graphics().get_logical_device().destroyPipeline(m_screen_shader.vk_graphics_pipeline);
		get_graphics().get_logical_device().destroyPipelineLayout(m_screen_shader.vk_pipeline_layout);
		get_graphics().get_logical_device().destroyShaderModule(m_screen_shader.vk_vertex_shader_module);
		get_graphics().get_logical_device().destroyShaderModule(m_screen_shader.vk_fragment_shader_module);
		get_graphics().get_logical_device().destroyDescriptorSetLayout(m_screen_shader.vk_descriptor_set_layout);

		Renderer::shutdown();
	}

	void ForwardRenderer::render()
	{
		// Wait for present queue to finish if needed
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		// Stop rendering if the main camera doesn't exist
		if (!m_main_camera.allocator)
		{
			flush_queues();
			return;
		}

		for (size_t i = 0; i < m_cameras->max_allocated(); ++i)
			if (m_cameras->is_allocated(i))
				render_to_camera(Handle<VirtualCamera>(i, m_cameras.get()));

		// Get image index to render too
		uint32_t image_index = get_graphics().get_logical_device().acquireNextImageKHR
		(
			get_swapchain_manager().get_swapchain(),
			std::numeric_limits<uint64_t>::max(),
			m_vk_image_available,
			vk::Fence()
		).value;

		// Prepare primary command buffer
		generate_primary_command_buffer(image_index);

		vk::SubmitInfo submit_info = {};

		std::array<vk::Semaphore, 2> wait_semaphores =
		{
			m_vk_image_available,
			m_vk_off_screen_rendering_finished
		};

		std::array<vk::PipelineStageFlags, 2> wait_stages = 
		{ 
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eColorAttachmentOutput
		};

		submit_info.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size());
		submit_info.pWaitSemaphores = wait_semaphores.data();
		submit_info.pWaitDstStageMask = wait_stages.data();
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &get_primary_command_buffer();
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_vk_on_screen_rendering_finished;

		// Submit to queue
		get_graphics().get_device_manager().get_graphics_queue().submit(submit_info, vk::Fence());

		vk::PresentInfoKHR present_info = {};
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &m_vk_on_screen_rendering_finished;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &get_swapchain_manager().get_swapchain();
		present_info.pImageIndices = &image_index;

		// Present to screen
		get_graphics().get_device_manager().get_present_queue().presentKHR(present_info);

		// Clear rendering queues
		flush_queues();
	}

	void ForwardRenderer::generate_primary_command_buffer(uint32_t image_index)
	{
		// Begin recording to primary command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		get_primary_command_buffer().begin(begin_info);

		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass = m_vk_on_screen_pass;
		render_pass_info.framebuffer = m_vk_framebuffers[image_index];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = get_swapchain_manager().get_image_extent();

		vk::ClearValue clear_color = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 1.0f };
		render_pass_info.clearValueCount = 2;
		render_pass_info.pClearValues = &clear_color;

		// Begin render pass
		get_primary_command_buffer().beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		// Set viewport
		vk::Viewport viewport = {};
		viewport.setHeight(static_cast<float>(get_graphics().get_height()));
		viewport.setWidth(static_cast<float>(get_graphics().get_width()));
		viewport.setMinDepth(0);
		viewport.setMaxDepth(1);
		get_primary_command_buffer().setViewport(0, 1, &viewport);

		// Set scissor
		vk::Rect2D scissor = {};
		scissor.setExtent(get_swapchain_manager().get_image_extent());
		scissor.setOffset({ 0, 0 });
		get_primary_command_buffer().setScissor(0, 1, &scissor);

		get_primary_command_buffer().bindPipeline(vk::PipelineBindPoint::eGraphics, m_screen_shader.vk_graphics_pipeline);

		const auto& mem_buffer = m_quad->get_vertex_buffer();
		vk::DeviceSize offsets[] = { 0 };

		get_primary_command_buffer().bindDescriptorSets
		(
			vk::PipelineBindPoint::eGraphics,
			m_screen_shader.vk_pipeline_layout,
			0,
			m_screen_shader.vk_descriptor_set,
			{}
		);

		get_primary_command_buffer().bindVertexBuffers(0, 1, &mem_buffer.buffer, offsets);
		get_primary_command_buffer().bindIndexBuffer(m_quad->get_index_buffer().buffer, 0, vk::IndexType::eUint16);
		get_primary_command_buffer().drawIndexed(static_cast<uint32_t>(m_quad->get_index_count()), 1, 0, 0, 0);

		// End render pass and command buffer
		get_primary_command_buffer().endRenderPass();
		get_primary_command_buffer().end();
	}

	void ForwardRenderer::render_to_camera(Handle<VirtualCamera> camera)
	{
		vk::CommandBufferBeginInfo cmd_buf_info = {};
		cmd_buf_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
		cmd_buf_info.pInheritanceInfo = nullptr;

		// Begin renderpass
		camera->command_buffer.get_command_buffer().begin(cmd_buf_info);

		// Clear values for all attachments written in the fragment shader
		std::array<vk::ClearValue, 2> clear_values;
		clear_values[0].setColor(std::array<float, 4>{ camera->clear_color.x, camera->clear_color.y, camera->clear_color.z, 0.0f });
		clear_values[1].setDepthStencil({ 1.0f, 0 });

		vk::RenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.renderPass = m_vk_shader_pass;
		render_pass_begin_info.framebuffer = camera->framebuffer;
		render_pass_begin_info.renderArea.extent = vk::Extent2D(camera->width, camera->height);
		render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
		render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_begin_info.pClearValues = clear_values.data();

		// Inheritance info for the meshes command buffers
		vk::CommandBufferInheritanceInfo inheritance_info = {};
		inheritance_info.renderPass = m_vk_shader_pass;
		inheritance_info.framebuffer = camera->framebuffer;

		camera->command_buffer.get_command_buffer().beginRenderPass(render_pass_begin_info, vk::SubpassContents::eSecondaryCommandBuffers);

		auto command_buffers = generate_renderable_command_buffers(inheritance_info);

		// Execute command buffers and perform lighting
		if (command_buffers.size() > 0)
			camera->command_buffer.get_command_buffer().executeCommands(command_buffers);

		camera->command_buffer.get_command_buffer().endRenderPass();
		camera->command_buffer.get_command_buffer().end();

		vk::SubmitInfo submit_info = {};
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &camera->command_buffer.get_command_buffer();
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_vk_off_screen_rendering_finished;

		// Submit draw command
		get_graphics().get_device_manager().get_graphics_queue().submit(1, &submit_info, { nullptr });
	}

	std::vector<vk::CommandBuffer> ForwardRenderer::generate_renderable_command_buffers(const vk::CommandBufferInheritanceInfo& inheritance_info)
	{
		std::vector<vk::CommandBuffer> command_buffers(m_renderable_objects.size());

		// Wait for threads to finish
		m_thread_pool->wait();

		// List of jobs.
		// First dimension is the queue.
		// Second dimension is the job list.
		std::vector<std::vector<std::function<void(void)>>> jobs(get_graphics().get_command_manager().get_pool_count());

		for (size_t i = 0; i < m_renderable_objects.size(); ++i)
		{
			auto& command_buffer = m_renderable_objects[i].command_buffer.get_command_buffer();
			command_buffers[i] = command_buffer;

			jobs[m_renderable_objects[i].command_buffer.get_thread_index()].push_back(([this, i, command_buffer, inheritance_info]()
			{
				vk::CommandBufferBeginInfo begin_info = {};
				begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue;
				begin_info.pInheritanceInfo = &inheritance_info;

				// Draw
				command_buffer.begin(begin_info);

				// Set viewport
				vk::Viewport viewport = {};
				viewport.setHeight(static_cast<float>(get_graphics().get_height()));
				viewport.setWidth(static_cast<float>(get_graphics().get_width()));
				viewport.setMinDepth(0);
				viewport.setMaxDepth(1);
				command_buffer.setViewport(0, 1, &viewport);

				// Set scissor
				vk::Rect2D scissor = {};
				scissor.setExtent(get_swapchain_manager().get_image_extent());
				scissor.setOffset({ 0, 0 });
				command_buffer.setScissor(0, 1, &scissor);

				command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_renderable_objects[i].shader->get_graphics_pipeline());

				const auto& mem_buffer = m_renderable_objects[i].mesh->get_vertex_buffer();
				vk::DeviceSize offsets[] = { 0 };

				command_buffer.bindDescriptorSets
				(
					vk::PipelineBindPoint::eGraphics,
					m_renderable_objects[i].shader->get_pipeline_layout(),
					0,
					static_cast<uint32_t>(m_renderable_objects[i].descriptor_sets.size()),
					m_renderable_objects[i].descriptor_sets.data(),
					0,
					nullptr
				);

				command_buffer.bindVertexBuffers(0, 1, &mem_buffer.buffer, offsets);
				command_buffer.bindIndexBuffer(m_renderable_objects[i].mesh->get_index_buffer().buffer, 0, vk::IndexType::eUint16);
				command_buffer.drawIndexed(static_cast<uint32_t>(m_renderable_objects[i].mesh->get_index_count()), 1, 0, 0, 0);
				command_buffer.end();
			}));
		}

		for (size_t i = 0; i < jobs.size(); ++i)
			m_thread_pool->workers[i]->add_jobs(jobs[i]);

		// Wait for threads to finish
		m_thread_pool->wait();

		return command_buffers;
	}

	Handle<VirtualCamera> ForwardRenderer::create_camera()
	{
		// Resize the array if necessary
		if (m_cameras->num_allocated() == m_cameras->max_allocated())
			m_cameras->resize(m_cameras->max_allocated() + 4);

		// Allocate camera and call constructor
		auto camera = Handle<VirtualCamera>(m_cameras->allocate(), m_cameras.get());
		::new(m_cameras->get_resource_by_handle(camera.id))(VirtualCamera)();

		camera->width = get_graphics().get_width();
		camera->height = get_graphics().get_height();

		camera->command_buffer = get_graphics().get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::ePrimary);

		// Albedo (color)
		FrameBufferAttachment color = get_graphics().create_attachment
		(
			get_swapchain_manager().get_image_format(),
			vk::ImageUsageFlagBits::eColorAttachment
		);

		// Depth attachment
		FrameBufferAttachment depth = get_graphics().create_attachment
		(
			find_best_depth_format(get_graphics().get_physical_device()),
			vk::ImageUsageFlagBits::eDepthStencilAttachment
		);

		// Create sampler to sample from the attachments
		vk::SamplerCreateInfo sampler = {};
		sampler.setMagFilter(vk::Filter::eNearest);
		sampler.setMinFilter(vk::Filter::eNearest);
		sampler.setMipmapMode(vk::SamplerMipmapMode::eLinear);
		sampler.setAddressModeU(vk::SamplerAddressMode::eClampToEdge);
		sampler.setAddressModeV(sampler.addressModeU);
		sampler.setAddressModeW(sampler.addressModeU);
		sampler.setMipLodBias(0.0f);
		sampler.setMaxAnisotropy(1.0f);
		sampler.setMinLod(0.0f);
		sampler.setMaxLod(1.0f);
		sampler.setBorderColor(vk::BorderColor::eFloatOpaqueWhite);

		// Create samplers
		vk::Sampler color_sampler = get_graphics().get_logical_device().createSampler(sampler);
		vk::Sampler depth_sampler = get_graphics().get_logical_device().createSampler(sampler);

		// Resize allocator if needed
		if (m_texture_allocator->num_allocated() + 2 > m_texture_allocator->max_allocated())
			m_texture_allocator->resize(m_texture_allocator->max_allocated() + 2);

		// Create attachments
		camera->attachments.resize(2);
		camera->attachments[0] = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator); // Color
		camera->attachments[1] = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator); // Depth

		::new(m_texture_allocator->get_resource_by_handle(camera->attachments[0].id))(Texture)
		(
			&get_graphics(), 
			color.image, 
			color.view, 
			color_sampler, 
			color.memory, 
			vk::Filter::eNearest,
			camera->width,
			camera->height
		);

		::new(m_texture_allocator->get_resource_by_handle(camera->attachments[1].id))(Texture)
		(
			&get_graphics(), 
			depth.image, 
			depth.view, 
			depth_sampler, 
			depth.memory, 
			vk::Filter::eNearest,
			camera->width, 
			camera->height
		);

		std::array<vk::ImageView, 2> attachments;
		attachments[0] = camera->attachments[0]->get_image_view();
		attachments[1] = camera->attachments[1]->get_image_view();

		vk::FramebufferCreateInfo fbuf_create_info = {};
		fbuf_create_info.setPNext(nullptr);
		fbuf_create_info.setRenderPass(m_vk_shader_pass);
		fbuf_create_info.setPAttachments(attachments.data());
		fbuf_create_info.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
		fbuf_create_info.setWidth(camera->width);
		fbuf_create_info.setHeight(camera->height);
		fbuf_create_info.setLayers(1);

		// Create framebuffer
		camera->framebuffer = get_graphics().get_logical_device().createFramebuffer(fbuf_create_info);

		return camera;
	}

	void ForwardRenderer::set_main_camera(Handle<VirtualCamera> camera)
	{
		m_main_camera = camera;

		if (camera.allocator != nullptr)
		{
			// Update descriptor set
			vk::DescriptorImageInfo image_info = {};
			image_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
			image_info.imageView = camera->attachments[0]->get_image_view();
			image_info.sampler = camera->attachments[0]->get_sampler();
			
			vk::WriteDescriptorSet write = {};
			write.dstSet = m_screen_shader.vk_descriptor_set;
			write.dstBinding = 0;
			write.dstArrayElement = 0;
			write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			write.descriptorCount = 1;
			write.pImageInfo = &image_info;
			
			get_graphics().get_logical_device().updateDescriptorSets(1, &write, 0, nullptr);
		}
	}
}