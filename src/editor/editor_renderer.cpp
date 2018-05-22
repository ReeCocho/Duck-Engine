/**
 * @file editor_renderer.cpp
 * @brief Duck editor renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "imgui\imgui.h"
#include "editor_renderer.hpp"

namespace dk
{
	EditorRenderer::EditorRenderer()
	{

	}

	EditorRenderer::EditorRenderer(Graphics* graphics) : 
		Renderer(graphics),
		m_vk_framebuffers({})
	{
		// Create command pool
		vk::CommandPoolCreateInfo pool_info = {};
		pool_info.queueFamilyIndex = get_graphics().get_device_manager().get_queue_family_indices().graphics_family;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		m_vk_command_pool = get_graphics().get_logical_device().createCommandPool(pool_info);
		dk_assert(m_vk_command_pool);

		// Create command buffer
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = m_vk_command_pool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;
		auto command_buffers = get_graphics().get_logical_device().allocateCommandBuffers(alloc_info);

		m_vk_primary_command_buffer = command_buffers[0];

		m_swapchain_manager = std::make_unique<VkSwapchainManager>
		(
			get_graphics().get_physical_device(),
			get_graphics().get_logical_device(),
			get_graphics().get_surface(),
			get_graphics().get_width(),
			get_graphics().get_height()
		);

		// Create semaphores
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_vk_image_available = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		m_vk_rendering_finished = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);
		dk_assert(m_vk_rendering_finished);

		// Create the render pass
		{
			vk::AttachmentDescription attachment = {};
			attachment.format = m_swapchain_manager->get_image_format();
			attachment.samples = vk::SampleCountFlagBits::e1;
			attachment.loadOp = vk::AttachmentLoadOp::eClear;
			attachment.storeOp = vk::AttachmentStoreOp::eStore;
			attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			attachment.initialLayout = vk::ImageLayout::eUndefined;
			attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

			vk::AttachmentReference color_attachment = {};
			color_attachment.attachment = 0;
			color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &color_attachment;

			vk::SubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

			vk::RenderPassCreateInfo info = {};
			info.attachmentCount = 1;
			info.pAttachments = &attachment;
			info.subpassCount = 1;
			info.pSubpasses = &subpass;
			info.dependencyCount = 1;
			info.pDependencies = &dependency;

			m_vk_render_pass = graphics->get_logical_device().createRenderPass(info);
			dk_assert(m_vk_render_pass);
		}

		// Create framebuffers
		m_vk_framebuffers.resize(m_swapchain_manager->get_image_count());
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++)
		{
			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_vk_render_pass;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = &get_swapchain_manager().get_image_view(i);
			framebuffer_info.width = get_swapchain_manager().get_image_extent().width;
			framebuffer_info.height = get_swapchain_manager().get_image_extent().height;
			framebuffer_info.layers = 1;

			m_vk_framebuffers[i] = get_graphics().get_logical_device().createFramebuffer(framebuffer_info);
			dk_assert(m_vk_framebuffers[i]);
		}
	}

	void EditorRenderer::shutdown()
	{
		// Wait for presentation to finish
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		// Destroy render pass
		get_graphics().get_logical_device().destroyRenderPass(m_vk_render_pass);

		// Destroy command pool
		get_graphics().get_logical_device().destroyCommandPool(m_vk_command_pool);

		// Destroy semaphores
		get_graphics().get_logical_device().destroySemaphore(m_vk_image_available);
		get_graphics().get_logical_device().destroySemaphore(m_vk_rendering_finished);

		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			get_graphics().get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy swapchain
		m_swapchain_manager.reset();
	}

	void EditorRenderer::render()
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

		// Clear screen
		{
			// Begin command buffer
			vk::CommandBufferBeginInfo cmd_buf_info = {};
			cmd_buf_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
			cmd_buf_info.pInheritanceInfo = nullptr;

			m_vk_primary_command_buffer.begin(cmd_buf_info);

			// Clear values for all attachments written in the fragment shader
			vk::ClearValue clear_value = {};
			clear_value.color = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 0.0f };

			// Window extent
			vk::Extent2D extent = {};
			extent.width = static_cast<uint32_t>(get_graphics().get_width());
			extent.height = static_cast<uint32_t>(get_graphics().get_height());

			// Set viewport
			vk::Viewport viewport = {};
			viewport.setHeight(static_cast<float>(extent.height));
			viewport.setWidth(static_cast<float>(extent.width));
			viewport.setMinDepth(0);
			viewport.setMaxDepth(1);
			m_vk_primary_command_buffer.setViewport(0, 1, &viewport);

			// Set scissor
			vk::Rect2D scissor = {};
			scissor.setExtent(extent);
			scissor.setOffset({ 0, 0 });
			m_vk_primary_command_buffer.setScissor(0, 1, &scissor);

			// Begin render pass
			vk::RenderPassBeginInfo render_pass_begin_info = {};
			render_pass_begin_info.renderPass = m_vk_render_pass;
			render_pass_begin_info.framebuffer = m_vk_framebuffers[image_index];
			render_pass_begin_info.renderArea.extent = extent;
			render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
			render_pass_begin_info.clearValueCount = 1;
			render_pass_begin_info.pClearValues = &clear_value;

			m_vk_primary_command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
			m_vk_primary_command_buffer.endRenderPass();
			m_vk_primary_command_buffer.end();

			vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eAllGraphics;

			vk::SubmitInfo submit_info = {};
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &m_vk_primary_command_buffer;
			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores = &m_vk_rendering_finished;
			submit_info.pWaitDstStageMask = &wait_stage;
			submit_info.pWaitSemaphores = &m_vk_image_available;
			submit_info.waitSemaphoreCount = 1;

			// Submit draw command
			get_graphics().get_device_manager().get_graphics_queue().submit(1, &submit_info, { nullptr });
		}

		// Wait for graphics queue to finish rendering
		get_graphics().get_device_manager().get_graphics_queue().waitIdle();

		// Present to screen
		{
			vk::PresentInfoKHR present_info = {};
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &m_vk_rendering_finished;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &get_swapchain_manager().get_swapchain();
			present_info.pImageIndices = &image_index;

			get_graphics().get_device_manager().get_present_queue().presentKHR(present_info);
		}

		// ImGuiIO& io = ImGui::GetIO();
		// ImGui::NewFrame();
	}
}