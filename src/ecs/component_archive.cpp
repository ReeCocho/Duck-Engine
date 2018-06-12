/**
 * @file component_archive.cpp
 * @brief Component archiver source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "component_archive.hpp"

namespace dk
{
	ComponentArchive::ComponentArchive(bool writing) :
		ReflectionContext(),
		m_writing(writing)
	{

	}

	ComponentArchive::~ComponentArchive()
	{

	}
}