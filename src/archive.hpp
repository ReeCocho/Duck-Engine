#pragma once

/**
 * @file archive.hpp
 * @brief Archive header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <ecs\scene.hpp>
#include <vector>
#include "resource_manager.hpp"

namespace dk
{
	/**
	 * Scene archiving object.
	 */
	class SceneWriteArchive
	{
	public:

		/**
		 * Constructor.
		 * @param Scene to archive.
		 * @param Resource manager to use for certain handles.
		 * @param Write chunk size.
		 */
		SceneWriteArchive(Scene* scene, ResourceManager* resource_manager, size_t chunk_size = 512);

		/**
		 * Destructor.
		 */
		~SceneWriteArchive();

		/**
		 * Serialize everything.
		 */
		void serialize();

		/**
		 * Write data to the archive.
		 * @tparam Data type.
		 * @param Data.
		 */
		template<typename T>
		void write(T data)
		{
			// Expand archive if we need to
			while ((sizeof(T) + (intptr_t)m_write_head) - (intptr_t)m_data > m_data_length)
				expand_archive();
			
			// Copy data
			std::memcpy(m_write_head, &data, sizeof(T));

			// Move write head
			m_write_head = (void*)((intptr_t)m_write_head + sizeof(T));
		}

		/**
		 * Write a vector to the archive.
		 * @tparam Type held by the vector.
		 * @param Vector.
		 */
		template<typename T>
		void write(const std::vector<T>& data)
		{
			// Copy length
			write<uint32_t>(static_cast<uint32_t>(data.size()));

			// Write every object in the vector
			for (const auto& elem : data)
				write<T>(elem);
		}

	private:

		/**
		 * Allocate more memory for the data.
		 */
		void expand_archive();

		/** List of systems components. */
		std::vector<std::vector<ResourceID>> m_systems_components;

		/** Scene to serialize. */
		Scene* m_scene;

		/** Resource manager. */
		ResourceManager* m_resource_manager;
		
		/** Size of serialized data. */
		size_t m_data_length;

		/** Pointer to serialized data. */
		void* m_data;

		/** Write head. */
		void* m_write_head;

		/** Chunck size. */
		size_t m_chunk_size;
	};
}