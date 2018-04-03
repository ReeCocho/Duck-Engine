#pragma once

/**
 * @file swapchain_manager.hpp
 * @brief Vulkan swapchain manager.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\debugging.hpp>
#include "vulkan_utilities.hpp"

namespace dk
{
	/**
	 * @brief Vulkan swapchain manager.
	 */
	class VkSwapchainManager
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Physical device.
		 * @param Logical device.
		 * @param Surface.
		 * @param Preferred width.
		 * @param Preferred height.
		 */
		VkSwapchainManager
		(
			const vk::PhysicalDevice& physical_device, 
			const vk::Device& logical_device, 
			const vk::SurfaceKHR& surface, 
			int width, int height
		);

		/**
		 * @brief Destructor.
		 */
		~VkSwapchainManager();

		/**
		 * @brief Get swapchain.
		 * @return Swapchain.
		 */
		const vk::SwapchainKHR get_swapchain() const
		{
			return m_vk_swapchain;
		}

		/**
		 * @brief Get image at index n.
		 * @param Index n of image.
		 * @return Image at index n.
		 */
		const vk::Image& get_image(size_t n) const
		{
			dk_assert(n < m_vk_images.size());
			return m_vk_images[n];
		}

		/**
		 * @brief Get swapchain image format.
		 * @return Swapchain image format.
		 */
		vk::Format get_image_format() const
		{
			return m_vk_image_format;
		}

		/**
		 * @brief Get swapchain image extent.
		 * @return Swapchain image extent.
		 */
		vk::Extent2D get_image_extent() const
		{
			return m_vk_extent;
		}

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other swapchain manager.
		 */
		VkSwapchainManager(const VkSwapchainManager& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other swapchain manager.
		 * @return This.
		 */
		VkSwapchainManager& operator=(const VkSwapchainManager& other) { return *this; };



		/** Logical device. */
		const vk::Device& m_vk_logical_device;

		/** Swapchain. */
		vk::SwapchainKHR m_vk_swapchain;

		/** Swapchain images. */
		std::vector<vk::Image> m_vk_images;

		/** Format of swapchain images. */
		vk::Format m_vk_image_format;

		/** Size of swapchain images. */
		vk::Extent2D m_vk_extent;
	};
}