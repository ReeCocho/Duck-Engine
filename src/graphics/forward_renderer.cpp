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
}