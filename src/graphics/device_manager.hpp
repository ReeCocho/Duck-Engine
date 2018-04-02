#pragma once

/**
 * @file device_manager.hpp
 * @brief Vulkan device manager.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vulkan_utilities.hpp"

namespace dk
{
	/**
	 * @brief Vulkan device manager.
	 */
	class VkDeviceManager
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Vulkan instance.
		 * @param Surface.
		 * @param Layers.
		 * @param Device extensions.
		 */
		VkDeviceManager(const vk::Instance& instance, const vk::SurfaceKHR& surface, const std::vector<const char*>& layers, const std::vector<const char*>& extensions);

		/**
		 * @brief Destructor.
		 */
		~VkDeviceManager();

		/**
		 * @brief Get physical device.
		 * @return Physical device.
		 */
		const vk::PhysicalDevice& get_physical_device() const
		{
			return m_vk_physical_device;
		}

		/**
		 * @brief Get logical device.
		 * @return Logical device.
		 */
		const vk::Device& get_logical_deivce() const
		{
			return m_vk_logical_device;
		}

		/**
		 * @brief Get graphics queue.
		 * @return Graphics queue.
		 */
		const vk::Queue& get_graphics_queue() const
		{
			return m_vk_graphics_queue;
		}

		/**
		 * @brief Get presentation queue.
		 * @return Presentation queue.
		 */
		const vk::Queue& get_present_queue() const
		{
			return m_vk_present_queue;
		}

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other device manager.
		 */
		VkDeviceManager(const VkDeviceManager& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other device manager.
		 * @return This.
		 */
		VkDeviceManager& operator=(const VkDeviceManager& other) { return *this; };



		/** Vulkan instance. */
		const vk::Instance& m_vk_instance = {};

		/** Physical devices queue family indices. */
		QueueFamilyIndices m_queue_family_indices = {};

		/** Physical device. */
		vk::PhysicalDevice m_vk_physical_device = {};

		/** Logical device. */
		vk::Device m_vk_logical_device = {};

		/** Graphics queue. */
		vk::Queue m_vk_graphics_queue = {};

		/** Presentation queue. */
		vk::Queue m_vk_present_queue = {};
	};
}