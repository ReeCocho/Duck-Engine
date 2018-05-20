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
		m_swapchain_manager = std::make_unique<VkSwapchainManager>
		(
			get_graphics().get_physical_device(),
			get_graphics().get_logical_device(),
			get_graphics().get_surface(),
			get_graphics().get_width(),
			get_graphics().get_height()
		);

		// Create semaphore
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_vk_image_available = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);

		// Resize framebuffers
		m_vk_framebuffers.resize(m_swapchain_manager->get_image_count());

		// Tell IMGUI about the window
		ImGuiIO& io = ImGui::GetIO(); (void)io;
#ifdef _WIN32
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(graphics->get_window(), &wmInfo);
		io.ImeWindowHandle = wmInfo.info.win.window;
#else
		(void)window;
#endif

		// Setup style
		ImGui::StyleColorsDark();

		// Setup display size (every frame to accommodate for window resizing)
		int w = graphics->get_width();
		int h = graphics->get_height();
		io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

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
			dependency.srcAccessMask = vk::AccessFlagBits(0);
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

			vk::RenderPassCreateInfo info = {};
			info.attachmentCount = 1;
			info.pAttachments = &attachment;
			info.subpassCount = 1;
			info.pSubpasses = &subpass;
			info.dependencyCount = 1;
			info.pDependencies = &dependency;

			m_render_pass = graphics->get_logical_device().createRenderPass(info);
			dk_assert(m_render_pass);
		}

		// Create framebuffers
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++)
		{
			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_render_pass;
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
		get_graphics().get_logical_device().destroyRenderPass(m_render_pass);

		// Destroy semaphore
		get_graphics().get_logical_device().destroySemaphore(m_vk_image_available);

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

		// Wait for graphics queue to finish rendering
		get_graphics().get_device_manager().get_graphics_queue().waitIdle();

		// Present to screen
		{
			vk::PresentInfoKHR present_info = {};
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &m_vk_image_available;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &get_swapchain_manager().get_swapchain();
			present_info.pImageIndices = &image_index;

			get_graphics().get_device_manager().get_present_queue().presentKHR(present_info);
		}

		ImGuiIO& io = ImGui::GetIO();
		ImGui::NewFrame();
	}

	void EditorRenderer::draw(const RenderableObject& obj) {}
}