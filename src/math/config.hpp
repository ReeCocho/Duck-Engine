#pragma once

/**
 * @file config.hpp
 * @brief Configuration for math.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Pi constant. */
#define DUCK_PI 3.14159265358979323846f

/** Tell Duck Math whether to use SIMD operations or not. */
#define DUCK_USE_SIMD 1

/** Tell Duck Math whether you want to use radians or degrees. */
#define DUCK_USE_RADIANS 0

#if DUCK_USE_RADIANS
	#define DUCK_RAD_CONST 1.0f
#else
	#define DUCK_RAD_CONST (DUCK_PI/180.0f)
#endif