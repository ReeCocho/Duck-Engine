/**
 * @file forward_renderer.cpp
 * @brief Duck forward renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "forward_renderer.hpp"

namespace dk
{
	ForwardRenderer::ForwardRenderer(Graphics& graphics) : Renderer(graphics)
	{
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

		// Create render pass
		vk::RenderPassCreateInfo render_pass_info = {};
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;

		m_vk_shader_pass = get_graphics().get_logical_device().createRenderPass(render_pass_info);

		// Create framebuffers
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++) 
		{
			auto attachment = get_swapchain_manager().get_image_view(i);

			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_vk_shader_pass;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = &attachment;
			framebuffer_info.width = get_swapchain_manager().get_image_extent().width;
			framebuffer_info.height = get_swapchain_manager().get_image_extent().height;
			framebuffer_info.layers = 1;

			m_vk_framebuffers[i] = get_graphics().get_logical_device().createFramebuffer(framebuffer_info);
			dk_assert(m_vk_framebuffers[i]);
		}

	}

	ForwardRenderer::~ForwardRenderer()
	{

	}

	void ForwardRenderer::render()
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

		// Prepare primary command buffer
		generate_primary_command_buffer(image_index);

		vk::SubmitInfo submit_info = {};

		std::array<vk::PipelineStageFlags, 1> wait_stages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &m_vk_image_available;
		submit_info.pWaitDstStageMask = wait_stages.data();
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &get_primary_command_buffer();
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_vk_rendering_finished;

		// Submit to queue
		get_graphics().get_device_manager().get_graphics_queue().submit(submit_info, vk::Fence());

		vk::PresentInfoKHR present_info = {};
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &m_vk_rendering_finished;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &get_swapchain_manager().get_swapchain();
		present_info.pImageIndices = &image_index;

		// Present to screen
		get_graphics().get_device_manager().get_present_queue().presentKHR(present_info);
	}

	void ForwardRenderer::generate_primary_command_buffer(uint32_t image_index)
	{
		// Begin recording to primary command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		get_primary_command_buffer().begin(begin_info);

		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass = m_vk_shader_pass;
		render_pass_info.framebuffer = m_vk_framebuffers[image_index];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = get_swapchain_manager().get_image_extent();

		vk::ClearValue clear_color = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 1.0f };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		// Begin render pass
		get_primary_command_buffer().beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		// vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		// vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		// End render pass and command buffer
		get_primary_command_buffer().endRenderPass();
		get_primary_command_buffer().end();
	}
}