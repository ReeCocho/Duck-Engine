/**
 * @file renderer.cpp
 * @brief Duck renderer base source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <array>
#include "renderer.hpp"

namespace dk
{
	Renderer::Renderer(Graphics& graphics) :	
		m_graphics(graphics), 
		m_swapchain_manager
		(
			m_graphics.get_physical_device(), 
			m_graphics.get_logical_device(), 
			m_graphics.get_surface(), 
			m_graphics.get_width(), 
			m_graphics.get_height()
		),
		m_vk_framebuffers(m_swapchain_manager.get_image_count())
	{
		
	}

	Renderer::~Renderer()
	{
		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			m_graphics.get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy shader render pass
		m_graphics.get_logical_device().destroyRenderPass(m_vk_shader_pass);
	}
}