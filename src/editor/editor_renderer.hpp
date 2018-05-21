#pragma once

/**
 * @file editor_renderer.hpp
 * @brief Duck editor renderer.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include <graphics\swapchain_manager.hpp>
#include <graphics/texture.hpp>
#include <graphics/renderer.hpp>

namespace dk
{
	/**
	 * Editor renderer class.
	 */
	class EditorRenderer : public Renderer
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		EditorRenderer();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 */
		EditorRenderer(Graphics* graphics);

		/**
		 * @brief Destructor.
		 */
		~EditorRenderer() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		void shutdown() override;

		/**
		 * @brief Render everything to the screen.
		 */
		void render() override;

		/**
		 * @brief Get swapchain manager.
		 * @return Swapchain manager.
		 */
		VkSwapchainManager& get_swapchain_manager()
		{
			return *m_swapchain_manager.get();
		}

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other rendering engine.
		 */
		EditorRenderer(const EditorRenderer& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other rendering engine.
		 * @return This.
		 */
		EditorRenderer& operator=(const EditorRenderer& other) { return *this; };



		/** Swapchain manager. */
		std::unique_ptr<VkSwapchainManager> m_swapchain_manager;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

		/** Semaphore to indicate an image is available for rendering too. */
		vk::Semaphore m_vk_image_available;

		/** Render pass. */
		vk::RenderPass m_render_pass;

		/**
		 * Editor shader.
		 */
		struct
		{
			/** Vertex shader module. */
			vk::ShaderModule vertex_shader_module;

			/** Fragment shader module. */
			vk::ShaderModule fragment_shader_module;

			/** Pipeline layout */
			vk::PipelineLayout layout;

			/** Pipeline. */
			vk::Pipeline pipeline;

		} m_shader;
	};
}