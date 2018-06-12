#pragma once

/**
 * @file hex.hpp
 * @brief Hexadecimal utilities header.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <vector>
#include <string>

namespace dk
{
	/**
	 * Convert a string of binary data into a hexadecimal string.
	 * @param Binary data.
	 * @param Binary data length.
	 * @return Hexadecimal string.
	 */
	extern std::string binary_to_hex(const char* data, size_t len);

	/**
	 * Convert a hexadecimal string into a list of characters.
	 * @param Hexadecimal string.
	 * @return Characters.
	 */
	extern std::vector<char> hex_to_binary(std::string hex);
}