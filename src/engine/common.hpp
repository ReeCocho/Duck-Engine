#pragma once

/**
 * @file common.hpp
 * @brief Common header files.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <engine\config.hpp>

// Engine/editor header
#if DK_EDITOR
#include <editor\editor.hpp>
#else
#include <engine\engine.hpp>
#endif