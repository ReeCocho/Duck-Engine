/**
 * @file device_manager.cpp
 * @brief Vulkan device manager source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <set>
#include <utilities\debugging.hpp>
#include "device_manager.hpp"

namespace dk
{
	VkDeviceManager::VkDeviceManager(const vk::Instance& instance, const vk::SurfaceKHR& surface, const std::vector<const char*>& layers, const std::vector<const char*>& extensions) : m_vk_instance(instance)
	{
		// Find physical device
		m_vk_physical_device = find_suitable_physical_device(m_vk_instance, extensions, surface);
		dk_assert(m_vk_physical_device);

		// Find queue family indices
		m_queue_family_indices = find_queue_family_indices(m_vk_physical_device, surface);
		dk_assert(m_queue_family_indices.is_complete());

		std::vector<vk::DeviceQueueCreateInfo> queue_create_infos = {};
		std::set<int> unique_queue_families = 
		{ 
			m_queue_family_indices.graphics_family, 
			m_queue_family_indices.present_family, 
			m_queue_family_indices.transfer_family 
		};

		float queue_priority = 1.0f;
		for (int queue_family : unique_queue_families)
		{
			vk::DeviceQueueCreateInfo queue_create_info = {};
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}

		vk::PhysicalDeviceFeatures device_features = {};

		// Create logical device
		vk::DeviceCreateInfo create_info = {};
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pEnabledFeatures = &device_features;
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();

		m_vk_logical_device = m_vk_physical_device.createDevice(create_info);
		dk_assert(m_vk_logical_device);

		// Get queues
		m_vk_graphics_queue = m_vk_logical_device.getQueue(static_cast<uint32_t>(m_queue_family_indices.graphics_family), 0);
		m_vk_present_queue = m_vk_logical_device.getQueue(static_cast<uint32_t>(m_queue_family_indices.present_family), 0);
		m_vk_transfer_queue = m_vk_logical_device.getQueue(static_cast<uint32_t>(m_queue_family_indices.transfer_family), 0);
	}

	VkDeviceManager::~VkDeviceManager()
	{
		// Destroy the logical device
		m_vk_logical_device.destroy();
	}
}