#pragma once

/**
 * @file get_device_manager().hpp
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
		 * @brief Default constructor.
		 */
		VkDeviceManager();

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
		vk::PhysicalDevice& get_physical_device()
		{
			return m_vk_physical_device;
		}

		/**
		 * @brief Get logical device.
		 * @return Logical device.
		 */
		vk::Device& get_logical_deivce()
		{
			return m_vk_logical_device;
		}

		/**
		 * @brief Get graphics queue.
		 * @return Graphics queue.
		 */
		vk::Queue& get_graphics_queue()
		{
			return m_vk_graphics_queue;
		}

		/**
		 * @brief Get presentation queue.
		 * @return Presentation queue.
		 */
		vk::Queue& get_present_queue()
		{
			return m_vk_present_queue;
		}

		/**
		 * @brief Get transfer queue.
		 * @return Transfer queue.
		 */
		vk::Queue& get_transfer_queue()
		{
			return m_vk_transfer_queue;
		}

		/**
		 * @brief Get queue family indices.
		 * @return Queue family indices.
		 */
		QueueFamilyIndices get_queue_family_indices() const
		{
			return m_queue_family_indices;
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

		/** Data transfer queue. */
		vk::Queue m_vk_transfer_queue = {};
	};
}