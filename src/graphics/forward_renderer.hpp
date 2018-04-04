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

	private:
	};
}