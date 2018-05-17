#pragma once

/**
 * @file forward_renderer.hpp
 * @brief Duck forward+ renderer.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include "swapchain_manager.hpp"
#include "lighting.hpp"
#include "texture.hpp"
#include "renderer.hpp"

namespace dk
{
	/**
	 * @brief Forward+ renderer.
	 * @note When drawing objects with this renderer, 
	 *       the first command buffer will be used for
	 *       renering, and the second will be used for
	 *       a depth prepass.
	 */
	class ForwardRenderer : public Renderer
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		ForwardRenderer();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Texture allocator.
		 * @param Mesh allocator.
		 */
		ForwardRenderer(Graphics* graphics, ResourceAllocator<Texture>* texture_allocator, ResourceAllocator<Mesh>* mesh_allocator);

		/**
		 * @brief Destructor.
		 */
		~ForwardRenderer() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		void shutdown() override;

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
			return m_render_passes.shader_pass;
		}

		/**
		 * @brief Get depth prepass used by shaders.
		 * @return Depth prepass.
		 */
		const vk::RenderPass& get_depth_prepass() const
		{
			return m_render_passes.depth_prepass;
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
		 * @brief Get descriptor set used by the renderer.
		 * @return Descriptor set.
		 */
		vk::DescriptorSet& get_descriptor_set()
		{
			return m_descriptor.set;
		}

		/**
		 * @brief Get descriptor set layout used by the renderer.
		 * @return Descriptor set layout.
		 */
		vk::DescriptorSetLayout& get_descriptor_set_layout()
		{
			return m_descriptor.layout;
		}

		/**
		 * @brief Render everything to the screen.
		 */
		void render() override;

		/**
		 * @brief Draw a renderable object.
		 * @param Renderable object.
		 */
		void draw(const RenderableObject& obj) override;

		/**
		 * @brief Draw a point light.
		 * @param Point light.
		 */
		void draw(const PointLightData& point_light)
		{
			m_lighting_manager->draw(point_light);
		}

		/**
		 * @brief Draw a directional light.
		 * @param Directional light.
		 */
		void draw(const DirectionalLightData& dir_light)
		{
			m_lighting_manager->draw(dir_light);
		}

		/**
		 * @brief Set main camera.
		 * @param Camera data.
		 */
		void set_main_camera(const CameraData& data) override
		{
			m_main_camera = data;
			m_lighting_manager->set_camera_position(data.position);
		}
	private:

		/**
		 * @brief Copy constructor.
		 * @param Other rendering engine.
		 */
		ForwardRenderer(const ForwardRenderer& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other rendering engine.
		 * @return This.
		 */
		ForwardRenderer& operator=(const ForwardRenderer& other) { return *this; };

		/**
		 * @brief Update lighting data.
		 */
		void upate_lighting_data();

		/**
		 * @brief Perform the depth prepass.
		 */
		void generate_depth_prepass_command_buffer();

		/**
		 * @brief Generate the rendering command buffer.
		 * @param Frame buffer to render too.
		 */
		void generate_rendering_command_buffer(const vk::Framebuffer& framebuffer);

		/**
		 * @brief Draw the cameras skybox to a command buffer.
		 * @param Command buffer.
		 * @param Inheritence info.
		 * @param Flag for depth prepass or no depth prepass.
		 */
		void draw_sky_box(VkManagedCommandBuffer& managed_command_buffer, vk::CommandBufferInheritanceInfo inherit_info, bool depth_prepass);

		/**
		 * @brief Clear the rendering queues.
		 */
		void flush_queues();



		/** Swapchain manager. */
		std::unique_ptr<VkSwapchainManager> m_swapchain_manager;
		
		/** Lighting manager. */
		std::unique_ptr<LightingManager> m_lighting_manager;

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

		/** Depth prepass command buffer. */
		vk::CommandBuffer m_vk_depth_prepass_command_buffer;

		/** Rendering command buffer. */
		vk::CommandBuffer m_vk_rendering_command_buffer;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

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

		/**
		 * @brief Render passes.
		 */
		struct
		{
			/** Render pass used for shaders. */
			vk::RenderPass shader_pass;

			/** Depth prepass used for shaders. */
			vk::RenderPass depth_prepass;
		} m_render_passes;

		/**
		 * @brief Semaphores.
		 */
		struct
		{
			/** Semaphore to indicate depth prepass has finished. */
			vk::Semaphore depth_prepass_finished;

			/** Semaphore to indicate on screen rendering has finished. */
			vk::Semaphore on_screen_rendering_finished;

			/** Semaphore to indicate an image is available for rendering too. */
			vk::Semaphore image_available;
		} m_semaphores;

		/**
		 * @brief ForwardRenderer descriptor.
		 */
		struct
		{
			/** Descriptor set layout. */
			vk::DescriptorSetLayout layout = {};

			/** Descriptor pool. */
			vk::DescriptorPool pool = {};

			/** Descriptor set. */
			vk::DescriptorSet set = {};

		} m_descriptor;
	};
}