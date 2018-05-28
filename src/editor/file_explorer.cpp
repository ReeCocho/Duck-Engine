/**
 * @file file_explorer.cpp
 * @brief File explorer source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "file_explorer.hpp"

namespace dk
{
	FileExplorer::FileExplorer(Graphics* graphics) : m_graphics(graphics)
	{

	}

	void FileExplorer::draw()
	{
		ImGui::Text("File explorer");
	}
}