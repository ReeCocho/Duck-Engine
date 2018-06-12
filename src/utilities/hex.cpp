/**
 * @file hex.cpp
 * @brief Hexadecimal utilities source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <unordered_map>
#include "debugging.hpp"
#include "hex.hpp"

namespace dk
{
	std::string binary_to_hex(const char* data, size_t len)
	{
		const char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		std::string str = "";
		str.resize(len * 2);

		for (size_t i = 0; i < len; ++i)
		{
			str[(i * 2) + 0] = hex_chars[(data[i] & 0xF0) >> 4];
			str[(i * 2) + 1] = hex_chars[data[i] & 0x0F];
		}

		return str;
	}

	std::vector<char> hex_to_binary(std::string hex)
	{
		dk_assert(hex.size() % 2 == 0);

		const std::unordered_map<char, char> hex_chars = 
		{ 
			{ '0',  0 }, { '1',  1 }, { '2',  2 }, { '3',  3 }, 
			{ '4',  4 }, { '5',  5 }, { '6',  6 }, { '7',  7 }, 
			{ '8',  8 }, { '9',  9 }, { 'A', 10 }, { 'B', 11 }, 
			{ 'C', 12 }, { 'D', 13 }, { 'E', 14 }, { 'F', 15 } 
		};

		// Binary data
		std::vector<char> binary(hex.size() / 2);

		for (size_t i = 0; i < binary.size(); ++i)
		{
			binary[i] = hex_chars.at(hex[i * 2]) << 4;
			binary[i] |= hex_chars.at(hex[(i * 2) + 1]);
		}

		return binary;
	}
}