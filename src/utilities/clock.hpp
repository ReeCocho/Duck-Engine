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
	 * @class Clock
	 * @brief Allows for timing
	 */
	class Clock
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Clock();

		/**
		 * @brief Destructor.
		 */
		~Clock() = default;

		/**
		 * @brief Get the time the clock was created.
		 * @return Time the clock was created.
		 * @note In seconds
		 */
		float get_creation_time() const
		{
			return	static_cast<float>(m_creation_time) / 
					static_cast<float>(SDL_GetPerformanceFrequency());
		}

		/**
		 * @brief Get elapsed time since the clock was created.
		 * @note In seconds.
		 */
		float get_elapsed_time() const
		{
			return	static_cast<float>(SDL_GetPerformanceCounter() - m_creation_time) / 
					static_cast<float>(SDL_GetPerformanceFrequency());
		}

		/**
		 * @brief Get time since last measuring delta time.
		 * @return Time since last measuring delta time.
		 * @note In seconds.
		 */
		float get_delta_time()
		{
			uint64_t new_time = SDL_GetPerformanceCounter();
			float delta_time =	static_cast<float>(new_time - m_measuring_time) / 
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