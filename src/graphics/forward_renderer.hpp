#pragma once

/**
 * @file forward_renderer.hpp
 * @brief Duck forward renderer.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
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
		 * @brief Constructor.
		 * @param Graphics context.
		 */
		ForwardRenderer(Graphics& graphics);

		/**
		 * @brief Destructor.
		 */
		~ForwardRenderer();

		/**
		 * @brief Render everything to the screen.
		 */
		void render() override;

	private:

		/**
		 * @brief Generate the primary command buffer for rendering.
		 * @param Image index.
		 */
		void generate_primary_command_buffer(uint32_t image_index);
	};
}