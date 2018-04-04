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
		m_vk_framebuffers({}),
		m_renderable_objects({})
	{
		// Resize framebuffers
		m_vk_framebuffers.resize(m_swapchain_manager.get_image_count());

		// Create command pool
		vk::CommandPoolCreateInfo pool_info = {};
		pool_info.queueFamilyIndex = m_graphics.get_device_manager().get_queue_family_indices().graphics_family;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		m_vk_command_pool = m_graphics.get_logical_device().createCommandPool(pool_info);
		dk_assert(m_vk_command_pool);

		// Create command buffer
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = m_vk_command_pool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;
		m_vk_primary_command_buffer = m_graphics.get_logical_device().allocateCommandBuffers(alloc_info)[0];
		dk_assert(m_vk_primary_command_buffer);

		// Create semaphores
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_vk_image_available = m_graphics.get_logical_device().createSemaphore(semaphore_info);
		m_vk_rendering_finished = m_graphics.get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);
		dk_assert(m_vk_rendering_finished);
	}

	Renderer::~Renderer()
	{
		// Wait for logical device
		m_graphics.get_device_manager().get_graphics_queue().waitIdle();
		m_graphics.get_device_manager().get_present_queue().waitIdle();
		m_graphics.get_logical_device().waitIdle();

		// Destroy semaphores
		m_graphics.get_logical_device().destroySemaphore(m_vk_image_available);
		m_graphics.get_logical_device().destroySemaphore(m_vk_rendering_finished);

		// Destroy command pool
		m_graphics.get_logical_device().destroyCommandPool(m_vk_command_pool);

		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			m_graphics.get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy shader render pass
		m_graphics.get_logical_device().destroyRenderPass(m_vk_shader_pass);
	}

	void Renderer::flush_queues()
	{
		m_renderable_objects.clear();
	}
}