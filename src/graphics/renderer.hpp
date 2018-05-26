#pragma once

/**
 * @file renderer.hpp
 * @brief Duck renderer base.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include "graphics.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "sky_box.hpp"

namespace dk
{
	/**
	 * @brief Renderer base class.
	 */
	class Renderer
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Renderer() = default;

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Width.
		 * @param Height.
		 */
		Renderer(Graphics* graphics, int width, int height);

		/**
		 * @brief Destructor.
		 */
		virtual ~Renderer() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		virtual void shutdown() = 0;

		/**
		 * @brief Get graphics context.
		 * @return Graphics context.
		 */
		Graphics& get_graphics() const
		{
			return *m_graphics;
		}

		/**
		 * @brief Render everything.
		 */
		virtual void render() = 0;

		/**
		 * Resize the window.
		 * @param New width.
		 * @param New height.
		 */
		virtual void resize(int width, int height);

		/**
		 * Get window width.
		 * @return Window width.
		 */
		int get_width() const
		{
			return m_width;
		}

		/**
		 * Get window height.
		 * @return Window height.
		 */
		int get_height() const
		{
			return m_height;
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



		/** Graphics context. */
		Graphics* m_graphics;

		/** Width. */
		int m_width;

		/** Height. */
		int m_height;
	};
}