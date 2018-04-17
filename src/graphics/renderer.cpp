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
		m_vk_off_screen_rendering_finished = m_graphics->get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);
		dk_assert(m_vk_on_screen_rendering_finished);
		dk_assert(m_vk_off_screen_rendering_finished);

		// Create camera allocator
		m_cameras = std::make_unique<ResourceAllocator<VirtualCamera>>(1);
	}

	void Renderer::shutdown()
	{
		// Wait for logical device
		m_graphics->get_device_manager().get_graphics_queue().waitIdle();
		m_graphics->get_device_manager().get_present_queue().waitIdle();
		m_graphics->get_logical_device().waitIdle();

		// Destroy camera allocator
		for (size_t i = 0; i < m_cameras->max_allocated(); ++i)
			if (m_cameras->is_allocated(i))
				destroy_camera(Handle<VirtualCamera>(i, m_cameras.get()));
		m_cameras.reset();

		// Destroy semaphores
		m_graphics->get_logical_device().destroySemaphore(m_vk_image_available);
		m_graphics->get_logical_device().destroySemaphore(m_vk_on_screen_rendering_finished);
		m_graphics->get_logical_device().destroySemaphore(m_vk_off_screen_rendering_finished);

		// Destroy command pool
		m_graphics->get_logical_device().destroyCommandPool(m_vk_command_pool);

		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			m_graphics->get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy render passes
		m_graphics->get_logical_device().destroyRenderPass(m_vk_shader_pass);
		m_graphics->get_logical_device().destroyRenderPass(m_vk_on_screen_pass);

		// Destroy swapchain
		m_swapchain_manager.reset();
	}

	void Renderer::flush_queues()
	{
		m_renderable_objects.clear();
	}

	void Renderer::destroy_camera(Handle<VirtualCamera> camera)
	{
		dk_assert(camera.allocator == m_cameras.get() && m_cameras->is_allocated(camera.id));

		camera->command_buffer.free();
		get_graphics().get_logical_device().destroyFramebuffer(camera->framebuffer);

		for (auto& attachment : camera->attachments)
		{
			attachment->free();
			m_texture_allocator->deallocate(attachment.id);
		}

		m_cameras->deallocate(camera.id);
	}
}