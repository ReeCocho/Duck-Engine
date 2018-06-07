#pragma once

/**
 * @file file_io.hpp
 * @brief Input and output commands for binary and text files.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <string>
#include <vector>

namespace dk
{
	/**
	 * Read a binary file.
	 * @param Path to the file.
	 * @return Binary data contained in the file.
	 */
	std::vector<char> read_binary_file(const std::string& path);
}