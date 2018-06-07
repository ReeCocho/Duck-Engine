#pragma once

/**
 * @file Clock.hpp
 * @brief Clock header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <SDL_timer.h>

namespace dk
{
	/**
	 * Allows for timing
	 */
	class Clock
	{
	public:

		/**
		 * Constructor.
		 */
		Clock();

		/**
		 * Destructor.
		 */
		~Clock() = default;

		/**
		 * Get the time the clock was created.
		 * @return Time the clock was created.
		 * @note In seconds
		 */
		inline float get_creation_time() const
		{
			return	static_cast<float>(m_creation_time) / 
					static_cast<float>(SDL_GetPerformanceFrequency());
		}

		/**
		 * Get elapsed time since the clock was created.
		 * @note In seconds.
		 */
		inline float get_elapsed_time() const
		{
			return	static_cast<float>(SDL_GetPerformanceCounter() - m_creation_time) / 
					static_cast<float>(SDL_GetPerformanceFrequency());
		}

		/**
		 * Get time since last measuring delta time.
		 * @return Time since last measuring delta time.
		 * @note In seconds.
		 */
		inline float get_delta_time()
		{
			const uint64_t new_time = SDL_GetPerformanceCounter();
			const float delta_time =	static_cast<float>(new_time - m_measuring_time) / 
										static_cast<float>(SDL_GetPerformanceFrequency());
			m_measuring_time = new_time;

			return delta_time;
		}

	private:

		/** Time the clock was created. */
		uint64_t m_creation_time;

		/** Last time the delta was measured. */
		uint64_t m_measuring_time;
	};
}