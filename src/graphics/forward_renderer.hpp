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

	private:

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



		/** Thread pool for rendering. */
		std::unique_ptr<ThreadPool> m_thread_pool;
	};
}