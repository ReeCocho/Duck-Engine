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
		 * @param Width.
		 * @param Height.
		 */
		EditorRenderer(Graphics* graphics, int width, int height);

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
		 * Resize the window.
		 * @param New width.
		 * @param New height.
		 */
		void resize(int width, int height);

		/**
		 * @brief Get swapchain manager.
		 * @return Swapchain manager.
		 */
		VkSwapchainManager& get_swapchain_manager()
		{
			return *m_swapchain_manager.get();
		}

		/**
		 * Get descriptor set layout.
		 * @return Descriptor set layout.
		 */
		vk::DescriptorSetLayout& get_descriptor_set_layout()
		{
			return m_vk_font_descriptor_set_layout;
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

		/**
		 * Rebuild the swapchain.
		 */
		void rebuild_swapchain();



		/** Swapchain manager. */
		std::unique_ptr<VkSwapchainManager> m_swapchain_manager;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

		/** Semaphore to indicate an image is available for rendering too. */
		vk::Semaphore m_vk_image_available;

		/** Semaphore to indicate rendering has finished. */
		vk::Semaphore m_vk_rendering_finished;

		/** Render pass. */
		vk::RenderPass m_vk_render_pass;

		/** Command pool. */
		vk::CommandPool m_vk_command_pool;

		/** Primary graphics command buffer. */
		vk::CommandBuffer m_vk_primary_command_buffer;

		/** UI shader. */
		std::unique_ptr<Shader> m_ui_shader;

		/** Font descriptor set layout. */
		vk::DescriptorSetLayout m_vk_font_descriptor_set_layout;

		/**
		 * Draw data.
		 */
		struct
		{
			/** Vertex buffer. */
			VkMemBuffer vertex_buffer;

			/** Size of data last sent to the vertex buffer. */
			size_t vertex_buffer_size = 0;

			/** Index buffer. */
			VkMemBuffer index_buffer;

			/** Size of data last sent to the index buffer. */
			size_t index_buffer_size = 0;

		} m_draw_data;
	};
}