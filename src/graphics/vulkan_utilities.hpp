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
	 * @brief Vulkan device queue family indices.
	 */
	struct QueueFamilyIndices 
	{
		int graphics_family = -1;

		int present_family = -1;

		/**
		 * @brief Check if all queues are avaliable.
		 * @return IF all queues are avaliable.
		 */
		bool is_complete() 
		{
			return graphics_family >= 0 && present_family >= 0;
		}
	};



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

	/**
	 * @brief Find a suitable physical device.
	 * @param Vulkan instance.
	 * @return The best physical device found.
	 * @note If no device was found, this function will return a VK_NULL_HANDLE.
	 */
	vk::PhysicalDevice find_suitable_physical_device(const vk::Instance& instance);

	/**
	 * @brief Find a physical devices queue family indices.
	 * @param Physical device.
	 * @param Surface.
	 * @return Devices queue family indices.
	 */
	QueueFamilyIndices find_queue_family_indices(const vk::PhysicalDevice& physical_device, const vk::SurfaceKHR& surface);
}