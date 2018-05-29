#pragma once

/**
 * @file archive.hpp
 * @brief Archive header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <json.hpp>
#include "debugging.hpp"

/** For convenience. */
using json = nlohmann::json;

namespace dk
{
	/**
	 * JSON based archive.
	 */
	class JSONArchive
	{
	public:

		/**
		 * Constructor.
		 * @note This will setup the archive for writing.
		 */
		JSONArchive();

		/**
		 * Constructor.
		 * @param JSON archive.
		 * @note This will setup the archive for reading.
		 */
		JSONArchive(const json& archive);

		/**
		 * Destructor.
		 */
		~JSONArchive() = default;

		/**
		 * Get if the archive is writing.
		 * @return If the archive is writing.
		 */
		bool is_writing() const
		{
			return m_writing;
		}

		/**
		 * String dump of the archive.
		 * @param Indentation.
		 * @return JSON string.
		 */
		std::string dump(int i) const
		{
			return m_json.dump(i);
		}

		/**
		 * Write data to the archive.
		 * @tparam Data type.
		 * @param Data name.
		 * @param Data.
		 */
		template<typename T>
		void write(const std::string& name, T data)
		{
			dk_assert(m_writing);
			m_json[name] = data;
		}

		/**
		 * Read data from the archive.
		 * @tparam Data type.
		 * @param Data name.
		 * @return Data.
		 */
		template<typename T>
		T read(const std::string& name)
		{
			dk_assert(!m_writing);
			return m_json[name];
		}

	private:

		/** Are we reading or writing? */
		bool m_writing;

		/** JSON object. */
		json m_json;
	};
}