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

/** Max number of point lights. */
#define DK_MAX_POINT_LIGHTS 128

/** Max number of directional lights. */
#define DK_MAX_DIRECTIONAL_LIGHTS 16

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
	};

	/**
	 * @brief An object that can be rendered onto the screen.
	 */
	struct RenderableObject
	{
		/** Command buffer to record to. */
		VkManagedCommandBuffer command_buffer;

		/** Depth prepass command buffer. */
		VkManagedCommandBuffer depth_prepass_command_buffer;

		/** Shader. */
		Handle<Shader> shader;

		/** Mesh. */
		Handle<Mesh> mesh;

		/** Descriptor sets. */
		std::vector<vk::DescriptorSet> descriptor_sets;

		/** Model matrix. */
		glm::mat4 model = {};
	};

	/**
	 * @brief Directional light data.
	 */
	struct DirectionalLightData
	{
		/** Direction. */
		glm::vec4 direction = {};

		/** Color. (W is intensity) */
		glm::vec4 color = {};
	};

	/**
	 * @brief Point light data.
	 */
	struct PointLightData
	{
		/** Position. (W is range) */
		glm::vec4 position = {};

		/** Color. (W is intensity) */
		glm::vec4 color = {};
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
		void render();

		/**
		 * @brief Submit a renderable object.
		 * @param Renderable object.
		 */
		void draw(const RenderableObject& ro)
		{
			m_renderable_objects.push_back(ro);
		}

		/**
		 * @brief Draw a point light.
		 * @param Point light.
		 */
		void draw(const PointLightData& point_light)
		{
			dk_assert(m_lighting_data.point_light_count + 1 < DK_MAX_POINT_LIGHTS);
			m_lighting_data.point_lights[m_lighting_data.point_light_count] = point_light;
			++m_lighting_data.point_light_count;
		}

		/**
		 * @brief Draw a directional light.
		 * @param Directional light.
		 */
		void draw(const DirectionalLightData& dir_light)
		{
			dk_assert(m_lighting_data.directional_light_count + 1 < DK_MAX_DIRECTIONAL_LIGHTS);
			m_lighting_data.directional_lights[m_lighting_data.directional_light_count] = dir_light;
			++m_lighting_data.directional_light_count;
		}

		/**
		 * @brief Set main camera.
		 * @param Camera data.
		 */
		void set_main_camera(const CameraData& data)
		{
			m_camera_data = data;
			m_lighting_data.camera_position = glm::vec4(data.position, 1.0f);
		}

		/**
		 * @brief Get main camera.
		 * @return Main camera
		 */
		const CameraData& get_main_camera() const
		{
			return m_camera_data;
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
		 * @brief Generate the rendering command buffer.
		 * @param Image index.
		 */
		void generate_rendering_command_buffer(uint32_t image_index);

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

		/** Depth prepass command buffer. */
		vk::CommandBuffer m_vk_depth_prepass_command_buffer;

		/** Rendering command buffer. */
		vk::CommandBuffer m_vk_rendering_command_buffer;

		/** List of renderable objects. */
		std::vector<RenderableObject> m_renderable_objects;

		/** Framebuffers. */
		std::vector<vk::Framebuffer> m_vk_framebuffers;

		/** Main camera data. */
		CameraData m_camera_data = {};

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
		 * @brief Lighting data
		 */
		struct
		{
			/** Point light data. */
			PointLightData point_lights[DK_MAX_POINT_LIGHTS] = {};

			/** Number of point lights used. */
			uint32_t point_light_count = 0;

			/** Padding */
			char padding1[12];

			/** Directional light data. */
			DirectionalLightData directional_lights[DK_MAX_DIRECTIONAL_LIGHTS] = {};

			/** Number of directional lights used. */
			uint32_t directional_light_count = 0;

			/** Padding */
			char padding2[12];

			/** Ambient color (W value is intensity.) */
			glm::vec4 ambient = { 1.0f, 1.0f, 1.0f, 0.2f };

			/** Camera position. */
			glm::vec4 camera_position = {};

		} m_lighting_data;

		/** Lighting uniform buffer. */
		VkMemBuffer m_lighting_ubo = {};

		/** Lighting UBO mapping pointer. */
		void* m_lighting_map = nullptr;

		/**
		 * @brief Renderer descriptor.
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