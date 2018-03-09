#pragma once

/**
 * @file config.hpp
 * @brief Configurationg for math.
 * @author Connor J. Bramham (ReeCocho)
 */

namespace dk
{
	/** Datatype used for real numbers. */
	using RealNumber = float;
}

/** Tell Duck Math whether to use SIMD operations or not. */
#define DUCK_USE_SIMD 1