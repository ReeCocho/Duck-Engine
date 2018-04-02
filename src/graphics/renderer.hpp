#pragma once

/**
 * @file renderer.hpp
 * @brief Duck renderer base.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "graphics.hpp"

namespace dk
{
	/**
	 * @brief Base renderer.
	 */
	class Renderer
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 */
		Renderer(Graphics* graphics);

		/**
		 * @brief Destructor.
		 */
		virtual ~Renderer();

	protected:

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



		/** Graphics context. */
		Graphics* m_graphics;

		/** Swapchain. */
		vk::SwapchainKHR m_vk_swapchain;
	};
}