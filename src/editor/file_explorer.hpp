#pragma once

/**
 * @file file_explorer.hpp
 * @brief File explorer header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <graphics\graphics.hpp>
#include "imgui\imgui.h"

namespace dk
{
	/**
	 * File explorer.
	 */
	class FileExplorer
	{
	public:

		/**
		 * Constructor.
		 * @param Graphics context.
		 */
		FileExplorer(Graphics* graphics);

		/**
		 * Destructor.
		 */
		~FileExplorer() = default;

		/**
		 * Draw the file explorer.
		 */
		void draw();

	private:

		/** Graphics context. */
		Graphics * m_graphics;
	};
}