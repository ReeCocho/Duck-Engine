#pragma once

/**
 * @file texture.hpp
 * @brief Different textures you can create.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\debugging.hpp>
#include "vulkan_utilities.hpp"

namespace dk
{
	/**
	 * @brief Base class for every texture
	 */
	class Texture
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Logical device.
		 * @param Image.
		 */
		// Texture(const vk::Device& logical_device, vk::Image image);

		/**
		 * @brief Destructor.
		 */
		// ~Texture();

	private:

		/** Logical device used to create the texture. */
		const vk::Device& m_vk_logical_deivce;

		/** Vulkan image. */
		vk::Image m_vk_image;
	};
}