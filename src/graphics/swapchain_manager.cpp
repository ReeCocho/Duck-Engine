/**
 * @file swapchain_manager.cpp
 * @brief Vulkan swapchain manager source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "swapchain_manager.hpp"

namespace dk
{
	VkSwapchainManager::VkSwapchainManager
	(
		const vk::PhysicalDevice& physical_device, 
		const vk::Device& logical_device, 
		const vk::SurfaceKHR& surface, 
		int width, int height
	) : m_vk_logical_device(logical_device)
	{
		// Get swapchain details
		SwapChainSupportDetails swap_chain_support = query_swap_chain_support(physical_device, surface);
		vk::SurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
		vk::PresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
		vk::Extent2D extent = choose_swap_extent(swap_chain_support.capabilities, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		// Get image count for presentation
		uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
		if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
			image_count = swap_chain_support.capabilities.maxImageCount;

		vk::SwapchainCreateInfoKHR create_info = {};
		create_info.surface = surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		QueueFamilyIndices indices = find_queue_family_indices(physical_device, surface);
		std::array<uint32_t, 2> queue_family_indices = { static_cast<uint32_t>(indices.graphics_family), static_cast<uint32_t>(indices.present_family) };

		if (indices.graphics_family != indices.present_family)
		{
			create_info.imageSharingMode = vk::SharingMode::eConcurrent;
			create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
			create_info.pQueueFamilyIndices = queue_family_indices.data();
		}
		else
		{
			create_info.imageSharingMode = vk::SharingMode::eExclusive;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}

		create_info.preTransform = swap_chain_support.capabilities.currentTransform;
		create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = vk::SwapchainKHR(nullptr);

		// Create swapchain
		m_vk_swapchain = m_vk_logical_device.createSwapchainKHR(create_info);
		dk_assert(m_vk_swapchain);

		// Get images
		m_vk_images = m_vk_logical_device.getSwapchainImagesKHR(m_vk_swapchain);

		// Store extent and surface format.
		m_vk_image_format = surface_format.format;
		m_vk_extent = extent;
	}

	VkSwapchainManager::~VkSwapchainManager()
	{
		// Cleanup
		m_vk_logical_device.destroySwapchainKHR(m_vk_swapchain);
	}
}