#pragma once

/**
 * @file renderer.hpp
 * @brief Duck renderer base.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "graphics.hpp"
#include "swapchain_manager.hpp"

namespace dk
{
	/**
	 * @brief Base renderer.
	 */
	class Renderer
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 */
		Renderer(Graphics& graphics);

		/**
		 * @brief Destructor.
		 */
		virtual ~Renderer();

		/**
		 * @brief Get graphics context.
		 * @return Graphics context.
		 */
		Graphics& get_graphics() const
		{
			return m_graphics;
		}

		/**
		 * @brief Get swapchain manager.
		 * @return Swapchain manager.
		 */
		VkSwapchainManager& get_swapchain_manager()
		{
			return m_swapchain_manager;
		}

		/**
		 * @brief Get render pass used by shaders.
		 * @return Render pass.
		 */
		const vk::RenderPass& get_shader_render_pass() const
		{
			return m_vk_shader_pass;
		}

		/**
		 * @brief Get primary graphics command buffer.
		 * @return Graphics command buffer.
		 */
		vk::CommandBuffer& get_primary_command_buffer()
		{
			return m_vk_primary_command_buffer;
		}

		/**
		 * @brief Render everything to the screen.
		 */
		virtual void render() = 0;

	protected:

		/** Render pass used for shaders. */
		vk::RenderPass m_vk_shader_pass;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

		/** Semaphore to indicate rendering has finished. */
		vk::Semaphore m_vk_rendering_finished;

		/** Semaphore to indicate an image is available for rendering too. */
		vk::Semaphore m_vk_image_available;

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other rendering engine.
		 */
		Renderer(const Renderer& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other rendering engine.
		 * @return This.
		 */
		Renderer& operator=(const Renderer& other) { return *this; };



		/** Graphics context. */
		Graphics& m_graphics;

		/** Swapchain manager. */
		VkSwapchainManager m_swapchain_manager;

		/** Command pool. */
		vk::CommandPool m_vk_command_pool;

		/** Primary graphics command buffer. */
		vk::CommandBuffer m_vk_primary_command_buffer;
	};
}