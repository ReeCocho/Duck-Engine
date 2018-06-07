#pragma once

/**
 * @file archive.hpp
 * @brief Archive header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <vector>
#include <string>
#include "debugging.hpp"
#include "reflection.hpp"

namespace dk
{
	/**
	 * Manages reflected data so that it can be stored in a persistant manner.
	 */
	class Archive : public ReflectionContext
	{
	public:

		/**
		 * Constructor.
		 * @param Size of chunks to allocate.
		 * @note This will setup the archive for writing.
		 */
		Archive(size_t chunk_size = 512);

		/**
		 * Constructor.
		 * @param Pointer to data.
		 * @param Length of data.
		 * @note This will setup the archive for reading.
		 */
		Archive(char* data, size_t len);

		/**
		 * Destructor.
		 */
		~Archive();

		/**
		 * Determine if the archive is in write mode.
		 * @return If the archive is in write mode.
		 */
		inline bool is_writing() const
		{
			return m_writing;
		}

		/**
		 * Get data pointer.
		 * @return Data pointer.
		 */
		inline char* get_data() const
		{
			return m_data;
		}

		/**
		 * Get number of bytes allocated.
		 * @return Number of bytes allocated.
		 */
		inline size_t get_bytes_allocated() const
		{
			return (size_t)((intptr_t)m_head - (intptr_t)m_data);
		}

		/**
		 * Write data to the archive.
		 * @tparam Data type.
		 * @param Data.
		 */
		template<typename T>
		void write(T data)
		{
			dk_assert(m_writing);

			// Expand archive if we need to
			while ((sizeof(T) + (intptr_t)m_head) - (intptr_t)m_data > m_data_length)
				expand_archive();
			
			// Copy data
			std::memcpy(m_head, &data, sizeof(T));

			// Move write head
			m_head = (char*)((intptr_t)m_head + sizeof(T));
		}

		/**
		 * Read data from the archive.
		 * @tparam Data type.
		 * @return Data.
		 */
		template<typename T>
		T read()
		{
			dk_assert(!m_writing && (size_t)((intptr_t)m_head - (intptr_t)m_data) + sizeof(T) <= m_data_length);
			void* old_head = m_head;
			m_head = (char*)((intptr_t)m_head + sizeof(T));
			return *(T*)(old_head);
		}

	private:

		/**
		 * Allocate more memory for the data.
		 */
		void expand_archive();

		/** Flag for a write archive. */
		bool m_writing;

		/** Size of serialized data. */
		size_t m_data_length;

		/** Pointer to serialized data. */
		char* m_data;

		/** Write/write head. */
		char* m_head;

		/** Chunck size. */
		size_t m_chunk_size;
	};

	// Specializations

	template<>
	void Archive::write<std::string>(std::string data);

	template<>
	std::string Archive::read<std::string>();
}