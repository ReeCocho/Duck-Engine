#pragma once

/**
 * @file forward_renderer.hpp
 * @brief Duck forward renderer.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include "renderer.hpp"

namespace dk
{
	/**
	 * @brief Default forward renderer.
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
		 * @brief Shutdown the forward renderer.
		 */
		void shutdown() override;

		/**
		 * @brief Render everything to the screen.
		 */
		void render() override;

		/**
		 * @brief Create a virtual camera
		 * @return Virtual camera.
		 */
		Handle<VirtualCamera> create_camera() override;

		/**
		 * @brief Change the main camera.
		 * @param New main camera.
		 */
		void set_main_camera(Handle<VirtualCamera> camera) override;

	private:

		/**
		 * @brief Generate the primary command buffer for rendering.
		 * @param Image index.
		 */
		void generate_primary_command_buffer(uint32_t image_index);

		/**
		 * @brief Render to camera.
		 * @param Camera handle.
		 */
		void render_to_camera(Handle<VirtualCamera> camera);

		/**
		 * @brief Generate command buffers for renderable objects.
		 * @param Inheritence info for the buffers.
		 * @return Vector of command buffers to submit.
		 */
		std::vector<vk::CommandBuffer> generate_renderable_command_buffers(const vk::CommandBufferInheritanceInfo& inheritance_info);



		/** Thread pool for rendering. */
		std::unique_ptr<ThreadPool> m_thread_pool;

		/** Screen quad. */
		Handle<Mesh> m_quad;

		/** Screen shader */
		struct
		{
			/** Vertex shader module. */
			vk::ShaderModule vk_vertex_shader_module;

			/** Fragment shader module. */
			vk::ShaderModule vk_fragment_shader_module;

			/** Descriptor set layout. */
			vk::DescriptorSetLayout vk_descriptor_set_layout;

			/** Descriptor pool. */
			vk::DescriptorPool vk_descriptor_pool;

			/** Descriptor set. */
			vk::DescriptorSet vk_descriptor_set;

			/** Graphics pipeline layout */
			vk::PipelineLayout vk_pipeline_layout;

			/** Graphics pipeline. */
			vk::Pipeline vk_graphics_pipeline;

		} m_screen_shader;
	};
}