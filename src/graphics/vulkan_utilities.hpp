#pragma once

/**
 * @file vulkan_utilities.hpp
 * @brief Common utilities for Vulkan.
 * @author Connor J. Bramham (ReeCocho)
 */

#define SDL_MAIN_HANDLED

/** Includes. */
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>
#include <vulkan\vulkan.hpp>
#include <vector>

#undef max
#undef min

namespace dk
{
	/**
	 * @brief Get a list of enabled layers.
	 * @param Requested layers
	 * @return Layers found.
	 */
	std::vector<const char*> get_layers(const std::vector<const char*>& layers);


	/**
	 * @brief Get a list of enabled extensions.
	 * @param Requested extensions
	 * @return Extensions found.
	 */
	std::vector<const char*> get_extensions(const std::vector<const char*>& extensions);
}