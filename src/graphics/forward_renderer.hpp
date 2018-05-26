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
	 * @brief Camera data structure.
	 */
	struct CameraData
	{
		/** View-projection matrix. */
		glm::mat4 vp_mat = {};

		/** Position in space. */
		glm::vec3 position = {};

		/** View frustum. */
		Frustum frustum = {};

		/** Command buffers to use for rendering the skybox. */
		std::vector<VkManagedCommandBuffer> command_buffers = {};

		/** Sky box. */
		Handle<SkyBox> sky_box = Handle<SkyBox>();
	};

	/**
	 * @brief An object that can be rendered onto the screen.
	 */
	struct RenderableObject
	{
		/** Command buffer to record to. */
		std::vector<VkManagedCommandBuffer> command_buffers = {};

		/** Shader. */
		Handle<MaterialShader> shader = {};

		/** Mesh. */
		Handle<Mesh> mesh = {};

		/** Descriptor sets. */
		std::vector<vk::DescriptorSet> descriptor_sets = {};

		/** Model matrix. */
		glm::mat4 model = {};
	};



	/**
	 * Forward+ renderer base class.
	 */
	class ForwardRendererBase : public Renderer
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		ForwardRendererBase();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Width.
		 * @param Height.
		 * @param Texture allocator.
		 * @param Mesh allocator.
		 */
		ForwardRendererBase
		(
			Graphics* graphics, 
			int width, 
			int height,
			ResourceAllocator<Texture>* texture_allocator, 
			ResourceAllocator<Mesh>* mesh_allocator
		);

		/**
		 * @brief Destructor.
		 */
		~ForwardRendererBase() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		virtual void shutdown() override;

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
		virtual void render() override = 0;

		/**
		 * @brief Draw a renderable object.
		 * @param Renderable object.
		 */
		void draw(const RenderableObject& obj);

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
		void set_main_camera(const CameraData& data)
		{
			m_main_camera = data;
			m_lighting_manager->set_camera_position(data.position);
		}

		/**
		 * @brief Get main camera.
		 * @return Main camera
		 */
		const CameraData& get_main_camera() const
		{
			return m_main_camera;
		}

	protected:

		/**
		 * @brief Copy constructor.
		 * @param Other rendering engine.
		 */
		ForwardRendererBase(const ForwardRendererBase& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other rendering engine.
		 * @return This.
		 */
		ForwardRendererBase& operator=(const ForwardRendererBase& other) { return *this; };

		/**
		 * @brief Update lighting data.
		 */
		void upate_lighting_data();

		/**
		 * @brief Perform the depth prepass.
		 * @param Size of window to draw on.
		 */
		void generate_depth_prepass_command_buffer(vk::Extent2D extent);

		/**
		 * @brief Generate the rendering command buffer.
		 * @param Frame buffer to render too.
		 * @param Size of window to draw on.
		 */
		void generate_rendering_command_buffer(const vk::Framebuffer& framebuffer, vk::Extent2D extent);

		/**
		 * @brief Draw the cameras skybox to a command buffer.
		 * @param Command buffer.
		 * @param Size of window to draw on.
		 * @param Inheritence info.
		 * @param Flag for depth prepass or no depth prepass.
		 */
		void draw_sky_box(VkManagedCommandBuffer& managed_command_buffer, vk::Extent2D extent, vk::CommandBufferInheritanceInfo inherit_info, bool depth_prepass);

		/**
		 * @brief Clear the rendering queues.
		 */
		void flush_queues();

		/**
		 * Create the depth image and framebuffer.
		 */
		void create_depth_data();

		/**
		 * Destroy the depth image and framebuffer.
		 */
		void destroy_depth_data();


		
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

		/** Main camera data. */
		CameraData m_main_camera;

		/** List of renderable objects. */
		std::vector<RenderableObject> m_renderable_objects;

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

			/** Semaphore to indicate color rendering has finished. */
			vk::Semaphore color_rendering_finished;
			
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

	/**
	 * Forward+ renderer.
	 * @note When drawing objects with this renderer, 
	 *       the first command buffer will be used for
	 *       renering, and the second will be used for
	 *       a depth prepass.
	 */
	class ForwardRenderer : public ForwardRendererBase
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
		 * @brief Render everything to the screen.
		 */
		void render() override;

		/**
		 * Resize the window.
		 * @param New width.
		 * @param New height.
		 */
		void resize(int width, int height) override;

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
		 * Create swapchain and framebuffers.
		 */
		void create_swapchain_data();

		/**
		 * Destroy swapchain and framebuffers.
		 */
		void destroy_swapchain_data();



		/** Swapchain manager. */
		std::unique_ptr<VkSwapchainManager> m_swapchain_manager;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

		/** Semaphore to indicate an image is available for rendering too. */
		vk::Semaphore m_vk_image_available;
	};



	/**
	 * Off screen Forward+ renderer.
	 * @note When drawing objects with this renderer, 
	 *       the first command buffer will be used for
	 *       renering, and the second will be used for
	 *       a depth prepass.
	 */
	class OffScreenForwardRenderer : public ForwardRendererBase
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		OffScreenForwardRenderer();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Width.
		 * @param Height.
		 * @param Texture allocator.
		 * @param Mesh allocator.
		 */
		OffScreenForwardRenderer
		(
			Graphics* graphics, 
			int width, 
			int height, 
			ResourceAllocator<Texture>* texture_allocator, 
			ResourceAllocator<Mesh>* mesh_allocator
		);

		/**
		 * @brief Destructor.
		 */
		~OffScreenForwardRenderer() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		void shutdown() override;

		/**
		 * Resize the window.
		 * @param New width.
		 * @param New height.
		 */
		void resize(int width, int height) override;

		/**
		 * @brief Render everything to the screen.
		 */
		void render() override;

		/**
		 * Get color texture.
		 * @return Color texture.
		 */
		Handle<Texture> get_color_texture() const
		{
			return m_color_texture;
		}

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other rendering engine.
		 */
		OffScreenForwardRenderer(const OffScreenForwardRenderer& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other rendering engine.
		 * @return This.
		 */
		OffScreenForwardRenderer& operator=(const OffScreenForwardRenderer& other) { return *this; };

		/**
		 * Create color texture and framebuffer.
		 */
		void create_color_data();

		/**
		 * Destroy color texture and framebuffer.
		 */
		void destroy_color_data();


		/** Color frame buffer. */
		vk::Framebuffer m_color_frame_buffer = {};

		/** Color exture to render to. */
		Handle<Texture> m_color_texture = {};
	};
}