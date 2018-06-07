/**
 * @file archive.cpp
 * @brief Archive source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "archive.hpp"

namespace dk
{
	Archive::Archive(size_t chunk_size) :
		m_chunk_size(chunk_size),
		m_writing(true)
	{
		// Allocate default chunk
		m_data = new char[m_chunk_size];
		m_data_length = m_chunk_size;
		m_head = m_data;
	}

	Archive::Archive(char* data, size_t len) :
		m_data(data),
		m_head(data),
		m_data_length(len),
		m_writing(false)
	{

	}

	Archive::~Archive()
	{
		// Free data
		if(m_writing) delete[] m_data;
	}

	void Archive::expand_archive()
	{
		dk_assert(m_writing);

		// Allocate new memory
		char* new_data = new char[m_data_length + m_chunk_size];

		// Copy old memory
		std::memcpy(new_data, m_data, m_data_length);

		// Move write head
		m_head = (char*)((intptr_t)new_data + ((intptr_t)m_head - (intptr_t)m_data));

		// Increase length
		m_data_length += m_chunk_size;

		// Delete old memory
		delete[] m_data;
		m_data = new_data;
	}

	template<>
	void Archive::write<std::string>(std::string data)
	{
		dk_assert(m_writing);

		// Write length
		write<uint32_t>(static_cast<uint32_t>(data.size()));

		// Write every character
		for (size_t i = 0; i < data.size(); ++i)
			write<char>(data[i]);
	}

	template<>
	std::string Archive::read<std::string>()
	{
		dk_assert(!m_writing);

		// Create string and read length
		std::string str = "";
		str.resize(read<uint32_t>());

		// Read every character
		for (size_t i = 0; i < str.size(); ++i)
			str[i] = read<char>();

		return str;
	}
}