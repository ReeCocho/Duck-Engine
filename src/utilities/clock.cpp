/**
 * @file Clock.cpp
 * @brief Clock source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "Clock.hpp"

namespace dk
{
	Clock::Clock()
	{
		m_creation_time = SDL_GetPerformanceCounter();
		m_measuring_time = m_creation_time;
	}
}