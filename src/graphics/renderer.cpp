/**
 * @file renderer.cpp
 * @brief Duck renderer base source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <array>
#include <utilities\file_io.hpp>
#include "renderer.hpp"

namespace dk
{
	Renderer::Renderer()
	{

	}

	Renderer::Renderer(Graphics* graphics, ResourceAllocator<Texture>* texture_allocator, ResourceAllocator<Mesh>* mesh_allocator) :
		m_graphics(graphics), 
		m_texture_allocator(texture_allocator),
		m_mesh_allocator(mesh_allocator),
		m_vk_framebuffers({}),
		m_renderable_objects({})
	{
		m_swapchain_manager = std::make_unique<VkSwapchainManager>
		(
			m_graphics->get_physical_device(),
			m_graphics->get_logical_device(),
			m_graphics->get_surface(),
			m_graphics->get_width(),
			m_graphics->get_height()
		);

		// Resize framebuffers
		m_vk_framebuffers.resize(m_swapchain_manager->get_image_count());

		// Create command pool
		vk::CommandPoolCreateInfo pool_info = {};
		pool_info.queueFamilyIndex = m_graphics->get_device_manager().get_queue_family_indices().graphics_family;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		m_vk_command_pool = m_graphics->get_logical_device().createCommandPool(pool_info);
		dk_assert(m_vk_command_pool);

		// Create command buffer
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = m_vk_command_pool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;
		m_vk_primary_command_buffer = m_graphics->get_logical_device().allocateCommandBuffers(alloc_info)[0];
		dk_assert(m_vk_primary_command_buffer);

		// Create semaphores
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_vk_image_available = m_graphics->get_logical_device().createSemaphore(semaphore_info);
		m_vk_on_screen_rendering_finished = m_graphics->get_logical_device().createSemaphore(semaphore_info);
		m_vk_depth_prepass_finished = m_graphics->get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);
		dk_assert(m_vk_on_screen_rendering_finished);
		dk_assert(m_vk_depth_prepass_finished);

		// Create depth prepass
		{
			// Describes depth attachment usage
			vk::AttachmentDescription attachment = {};
			attachment.format = find_best_depth_format(get_graphics().get_physical_device());
			attachment.samples = vk::SampleCountFlagBits::e1;
			attachment.loadOp = vk::AttachmentLoadOp::eClear;
			attachment.storeOp = vk::AttachmentStoreOp::eStore;
			attachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
			attachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
			attachment.finalLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;

			vk::AttachmentReference attachment_ref = {};
			attachment_ref.attachment = 0;
			attachment_ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 0;
			subpass.pColorAttachments = nullptr;
			subpass.pDepthStencilAttachment = &attachment_ref;

			vk::SubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
			dependency.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
			dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

			vk::RenderPassCreateInfo render_pass_info = {};
			render_pass_info.attachmentCount = 1;
			render_pass_info.pAttachments = &attachment;
			render_pass_info.subpassCount = 1;
			render_pass_info.pSubpasses = &subpass;
			render_pass_info.dependencyCount = 1;
			render_pass_info.pDependencies = &dependency;

			m_vk_depth_prepass = get_graphics().get_logical_device().createRenderPass(render_pass_info);
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
			attachment_descs[1].loadOp = vk::AttachmentLoadOp::eLoad;
			attachment_descs[1].storeOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[1].stencilLoadOp = vk::AttachmentLoadOp::eLoad;
			attachment_descs[1].stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[1].initialLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
			attachment_descs[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

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

		// Create depth image
		{
			auto depth_format = find_best_depth_format(get_graphics().get_physical_device());

			// Depth attachment
			FrameBufferAttachment depth = get_graphics().create_attachment
			(
				depth_format,
				vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled
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

			// Create sampler
			vk::Sampler depth_sampler = get_graphics().get_logical_device().createSampler(sampler);

			// Resize allocator if needed
			if (m_texture_allocator->num_allocated() + 1 > m_texture_allocator->max_allocated())
				m_texture_allocator->resize(m_texture_allocator->max_allocated() + 1);

			// Create attachments
			m_depth_prepass_image.depth_texture = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator);

			::new(m_texture_allocator->get_resource_by_handle(m_depth_prepass_image.depth_texture.id))(Texture)
			(
				&get_graphics(),
				depth.image,
				depth.view,
				depth_sampler,
				depth.memory,
				vk::Filter::eNearest,
				m_graphics->get_width(),
				m_graphics->get_height()
			);

			vk::FramebufferCreateInfo fbuf_create_info = {};
			fbuf_create_info.pNext = nullptr;
			fbuf_create_info.renderPass = m_vk_depth_prepass;
			fbuf_create_info.pAttachments = &m_depth_prepass_image.depth_texture->get_image_view();
			fbuf_create_info.attachmentCount = 1;
			fbuf_create_info.width = m_graphics->get_width();
			fbuf_create_info.height = m_graphics->get_height();
			fbuf_create_info.layers = 1;

			// Create framebuffer
			m_depth_prepass_image.framebuffer = get_graphics().get_logical_device().createFramebuffer(fbuf_create_info);

			m_graphics->transition_image_layout
			(
				m_depth_prepass_image.depth_texture->get_image(), 
				depth_format, 
				vk::ImageLayout::eUndefined, 
				vk::ImageLayout::eDepthStencilAttachmentOptimal
			);
		}

		// Create framebuffers
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++)
		{
			std::array<vk::ImageView, 2> attachments =
			{
				get_swapchain_manager().get_image_view(i),
				m_depth_prepass_image.depth_texture->get_image_view()
			};

			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_vk_shader_pass;
			framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = get_swapchain_manager().get_image_extent().width;
			framebuffer_info.height = get_swapchain_manager().get_image_extent().height;
			framebuffer_info.layers = 1;

			m_vk_framebuffers[i] = get_graphics().get_logical_device().createFramebuffer(framebuffer_info);
			dk_assert(m_vk_framebuffers[i]);
		}

		// Create thread pool
		m_thread_pool = std::make_unique<ThreadPool>(get_graphics().get_command_manager().get_pool_count());
	}

	void Renderer::shutdown()
	{
		// Wait for logical device
		m_graphics->get_device_manager().get_graphics_queue().waitIdle();
		m_graphics->get_device_manager().get_present_queue().waitIdle();
		m_graphics->get_logical_device().waitIdle();

		// Destroy thread pool
		m_thread_pool->wait();
		m_thread_pool.reset();

		// Destroy semaphores
		m_graphics->get_logical_device().destroySemaphore(m_vk_image_available);
		m_graphics->get_logical_device().destroySemaphore(m_vk_on_screen_rendering_finished);
		m_graphics->get_logical_device().destroySemaphore(m_vk_depth_prepass_finished);

		// Depth depth image
		get_graphics().get_logical_device().destroyFramebuffer(m_depth_prepass_image.framebuffer);
		m_depth_prepass_image.depth_texture->free();
		m_texture_allocator->deallocate(m_depth_prepass_image.depth_texture.id);

		// Destroy command pool
		m_graphics->get_logical_device().destroyCommandPool(m_vk_command_pool);

		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			m_graphics->get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy render passes
		m_graphics->get_logical_device().destroyRenderPass(m_vk_shader_pass);
		m_graphics->get_logical_device().destroyRenderPass(m_vk_depth_prepass);

		// Destroy swapchain
		m_swapchain_manager.reset();
	}

	void Renderer::flush_queues()
	{
		m_renderable_objects.clear();
	}

	void Renderer::render()
	{
		// Wait for present queue to finish if needed
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		// Get image index to render too
		uint32_t image_index = get_graphics().get_logical_device().acquireNextImageKHR
		(
			get_swapchain_manager().get_swapchain(),
			std::numeric_limits<uint64_t>::max(),
			m_vk_image_available,
			vk::Fence()
		).value;

		// Perform depth prepass
		do_depth_prepass();

		// Prepare primary command buffer
		generate_primary_command_buffer(image_index);

		vk::SubmitInfo submit_info = {};

		std::array<vk::Semaphore, 2> wait_semaphores =
		{
			m_vk_image_available,
			m_vk_depth_prepass_finished
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
		submit_info.pCommandBuffers = &m_vk_primary_command_buffer;
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

	void Renderer::do_depth_prepass()
	{
		vk::CommandBufferBeginInfo cmd_buf_info = {};
		cmd_buf_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
		cmd_buf_info.pInheritanceInfo = nullptr;

		// Begin renderpass
		m_vk_primary_command_buffer.begin(cmd_buf_info);

		// Clear values for all attachments written in the fragment shader
		vk::ClearValue clear_value = {};
		clear_value.setDepthStencil({ 1.0f, 0 });

		vk::RenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.renderPass = m_vk_depth_prepass;
		render_pass_begin_info.framebuffer = m_depth_prepass_image.framebuffer;
		render_pass_begin_info.renderArea.extent = vk::Extent2D(m_graphics->get_width(), m_graphics->get_height());
		render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = &clear_value;

		// Inheritance info for the meshes command buffers
		vk::CommandBufferInheritanceInfo inheritance_info = {};
		inheritance_info.renderPass = m_vk_depth_prepass;
		inheritance_info.framebuffer = m_depth_prepass_image.framebuffer;

		m_vk_primary_command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eSecondaryCommandBuffers);

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

				command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_renderable_objects[i].shader->get_depth_pipeline());

				const auto& mem_buffer = m_renderable_objects[i].mesh->get_vertex_buffer();
				vk::DeviceSize offsets[] = { 0 };

				command_buffer.bindDescriptorSets
				(
					vk::PipelineBindPoint::eGraphics,
					m_renderable_objects[i].shader->get_depth_pipeline_layout(),
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

		// Execute command buffers and perform lighting
		if (command_buffers.size() > 0)
			m_vk_primary_command_buffer.executeCommands(command_buffers);

		m_vk_primary_command_buffer.endRenderPass();
		m_vk_primary_command_buffer.end();

		vk::SubmitInfo submit_info = {};
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &m_vk_primary_command_buffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_vk_depth_prepass_finished;

		// Submit draw command
		get_graphics().get_device_manager().get_graphics_queue().submit(1, &submit_info, { nullptr });
		get_graphics().get_device_manager().get_graphics_queue().waitIdle();
	}

	void Renderer::generate_primary_command_buffer(uint32_t image_index)
	{
		// Begin recording to primary command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		m_vk_primary_command_buffer.begin(begin_info);

		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass = m_vk_shader_pass;
		render_pass_info.framebuffer = m_vk_framebuffers[image_index];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = get_swapchain_manager().get_image_extent();

		// Inheritance info for the meshes command buffers
		vk::CommandBufferInheritanceInfo inheritance_info = {};
		inheritance_info.renderPass = m_vk_shader_pass;
		inheritance_info.framebuffer = m_vk_framebuffers[image_index];

		vk::ClearValue clear_color = {};  
		clear_color.color = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 1.0f };

		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		// Begin render pass
		m_vk_primary_command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eSecondaryCommandBuffers);

		auto command_buffers = generate_renderable_command_buffers(inheritance_info);

		if (command_buffers.size() > 0)
			m_vk_primary_command_buffer.executeCommands(command_buffers);

		// End render pass and command buffer
		m_vk_primary_command_buffer.endRenderPass();
		m_vk_primary_command_buffer.end();
	}

	std::vector<vk::CommandBuffer> Renderer::generate_renderable_command_buffers(const vk::CommandBufferInheritanceInfo& inheritance_info)
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
					m_renderable_objects[i].shader->get_graphics_pipeline_layout(),
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
}