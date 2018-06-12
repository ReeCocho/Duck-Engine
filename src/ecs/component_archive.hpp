#pragma once

/**
 * @file component_archive.hpp
 * @brief Component archiver header.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\reflection.hpp>

namespace dk
{
	/**
	 * Component archiver.
	 */
	class ComponentArchive : public ReflectionContext
	{
	public:

		/**
		 * Constructor.
		 * @param Is the archive in write mode?
		 */
		ComponentArchive(bool writing);

		/**
		 * Destructor.
		 */
		~ComponentArchive();

		/**
		 * If the archive is in write mode or not.
		 * @return If the archive is in write mode or not.
		 */
		inline bool is_writing() const
		{
			return m_writing;
		}

	private:

		/** If the archive is in write mode. */
		bool m_writing;
	};
}