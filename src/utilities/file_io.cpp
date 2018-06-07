/**
 * @file file_io.cpp
 * @brief File IO source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <fstream>
#include "file_io.hpp"
#include "debugging.hpp"

namespace dk
{
	std::vector<char> read_binary_file(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		dk_assert(file.is_open());

		const size_t file_size = static_cast<size_t>(file.tellg());
		std::vector<char> data(file_size);

		file.seekg(0);
		file.read(data.data(), file_size);

		file.close();

		return data;
	}
}