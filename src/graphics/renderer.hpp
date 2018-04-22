#pragma once

/**
 * @file renderer.hpp
 * @brief Duck renderer base.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include <utilities\threading.hpp>
#include "graphics.hpp"
#include "swapchain_manager.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"

namespace dk
{
	/**
	 * @brief An object that can be rendered onto the screen.
	 */
	struct RenderableObject
	{
		/** Command buffer to record to. */
		VkManagedCommandBuffer command_buffer;

		/** Shader. */
		Handle<Shader> shader;

		/** Mesh. */
		Handle<Mesh> mesh;

		/** Descriptor sets. */
		std::vector<vk::DescriptorSet> descriptor_sets;
	};



	/**
	 * @brief Base renderer.
	 */
	class Renderer
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Renderer();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Texture allocator.
		 * @param Mesh allocator.
		 */
		Renderer(Graphics* graphics, ResourceAllocator<Texture>* texture_allocator, ResourceAllocator<Mesh>* mesh_allocator);

		/**
		 * @brief Destructor.
		 */
		~Renderer() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		void shutdown();

		/**
		 * @brief Get graphics context.
		 * @return Graphics context.
		 */
		Graphics& get_graphics() const
		{
			return *m_graphics;
		}

		/**
		 * @brief Get swapchain manager.
		 * @return Swapchain manager.
		 */
		VkSwapchainManager& get_swapchain_manager()
		{
			return *m_swapchain_manager.get();
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
		 * @brief Get depth prepass used by shaders.
		 * @return Depth prepass.
		 */
		const vk::RenderPass& get_depth_prepass() const
		{
			return m_vk_depth_prepass;
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
		void render();

		/**
		 * @brief Submit a renderable object.
		 * @param Renderable object.
		 */
		void draw(const RenderableObject& ro)
		{
			m_renderable_objects.push_back(ro);
		}

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

		/**
		 * @brief Perform the depth prepass.
		 */
		void do_depth_prepass();

		/**
		 * @brief Generate the primary command buffer for rendering.
		 * @param Image index.
		 */
		void generate_primary_command_buffer(uint32_t image_index);

		/**
		 * @brief Generate command buffers for renderable objects.
		 * @param Inheritence info for the buffers.
		 * @return Vector of command buffers to submit.
		 */
		std::vector<vk::CommandBuffer> generate_renderable_command_buffers(const vk::CommandBufferInheritanceInfo& inheritance_info);

		/**
		 * @brief Clear the rendering queues.
		 */
		void flush_queues();



		/** Graphics context. */
		Graphics* m_graphics;

		/** Swapchain manager. */
		std::unique_ptr<VkSwapchainManager> m_swapchain_manager;

		/** Texture allocator. */
		ResourceAllocator<Texture>* m_texture_allocator;

		/** Mesh allocator. */
		ResourceAllocator<Mesh>* m_mesh_allocator;

		/** Thread pool for rendering. */
		std::unique_ptr<ThreadPool> m_thread_pool;

		/** Command pool. */
		vk::CommandPool m_vk_command_pool;

		/** Primary graphics command buffer. */
		vk::CommandBuffer m_vk_primary_command_buffer;

		/** List of renderable objects. */
		std::vector<RenderableObject> m_renderable_objects;

		/** Render pass used for shaders. */
		vk::RenderPass m_vk_shader_pass;

		/** Depth prepass used for shaders. */
		vk::RenderPass m_vk_depth_prepass;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

		/** Semaphore to indicate depth prepass has finished. */
		vk::Semaphore m_vk_depth_prepass_finished;

		/** Semaphore to indicate on screen rendering has finished. */
		vk::Semaphore m_vk_on_screen_rendering_finished;

		/** Semaphore to indicate an image is available for rendering too. */
		vk::Semaphore m_vk_image_available;

		/**
		 * @brief Depth prepass image.
		 */
		struct
		{
			/** Framebuffer. */
			vk::Framebuffer framebuffer = {};

			/** Depth texture. */
			Handle<Texture> depth_texture = {};

		} m_depth_prepass_image;
	};
}