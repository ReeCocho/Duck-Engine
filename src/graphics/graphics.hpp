#pragma once

/**
 * @file graphics.hpp
 * @brief Vulkan graphics context.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <string>
#include <utilities\debugging.hpp>
#include "vulkan_utilities.hpp"

namespace dk
{
	/**
	 * @brief Vulkan graphics context.
	 */
	class Graphics
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Window name.
		 * @param Window width.
		 * @param Window height.
		 */
		Graphics(const std::string& name, int width, int height);

		/**
		 * @brief Destructor.
		 */
		~Graphics();

		/**
		 * @brief Get the SDL window.
		 * @return The SDL window.
		 */
		const SDL_Window* get_window() const
		{
			return m_window;
		}

		/**
		 * @brief Get the Vulkan instance.
		 * @return The Vulkan instance.
		 */
		const vk::Instance& get_vk_instance() const
		{
			return m_vk_instance;
		}

		/**
		 * @brief Get the name of the window.
		 * @return Name of the window.
		 */
		const std::string& get_name() const
		{
			return m_name;
		}

		/**
		 * @brief Get the width of the window.
		 * @return Window width.
		 */
		int get_width() const
		{
			int w, h;
			SDL_GetWindowSize(m_window, &w, &h);
			return w;
		}

		/**
		 * @brief Get the height of the window.
		 * @return Window height.
		 */
		int get_height() const
		{
			int w, h;
			SDL_GetWindowSize(m_window, &w, &h);
			return h;
		}

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other graphics context.
		 */
		Graphics(const Graphics& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other graphics context.
		 * @return This.
		 */
		Graphics& operator=(const Graphics& other) { return *this; };



		/** SDL window. */
		SDL_Window* m_window;

		/** Window name. */
		std::string m_name;

		/** Vulkan instance. */
		vk::Instance m_vk_instance;
	};
}