#pragma once

/**
 * @file texture.hpp
 * @brief Textures header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\debugging.hpp>
#include "graphics.hpp"

namespace dk
{
	/**
	 * @brief Base class for every texture
	 */
	class Texture
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Texture() = default;

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Image
		 * @param Image view.
		 * @param Sampler.
		 * @param Image memory.
		 * @param Filtering.
		 * @param Width.
		 * @param Height.
		 */
		Texture
		(
			Graphics* graphics, 
			vk::Image image,
			vk::ImageView image_view,
			vk::Sampler sampler,
			vk::DeviceMemory memory,
			vk::Filter filter,
			uint32_t width,
			uint32_t height
		);

		/**
		 * @brief Graphics context.
		 * @param Path to file containing image.
		 * @param Filtering.
		 */
		Texture(Graphics* graphics, const std::string& path, vk::Filter filtering);

		/**
		 * @brief Destructor.
		 */
		~Texture() = default;

		/**
		 * @brief Free texture resources.
		 */
		void free();

		/**
		 * @brief Get image width.
		 * @return Width.
		 */
		uint32_t get_width() const
		{
			return m_width;
		}

		/**
		 * @brief Get image height.
		 * @return Height.
		 */
		uint32_t get_height() const
		{
			return m_height;
		}

		/**
		 * @brief Get image view.
		 * @return Image view.
		 */
		vk::ImageView& get_image_view()
		{
			return m_vk_image_view;
		}

		/**
		 * @brief Get sampler.
		 * @return Sampler.
		 */
		vk::Sampler& get_sampler()
		{
			return m_vk_sampler;
		}

		/**
		 * @brief Get filtering.
		 * @return Filtering.
		 */
		vk::Filter get_filtering() const
		{
			return m_vk_filtering;
		}

		/**
		 * @brief Get image.
		 * @return Image.
		 */
		vk::Image& get_image()
		{
			return m_vk_image;
		}

	private:

		/** Graphics context */
		Graphics* m_graphics;

		/** Vulkan image. */
		vk::Image m_vk_image;

		/** Vulkan image memory. */
		vk::DeviceMemory m_vk_memory;

		/** Texture image view. */
		vk::ImageView m_vk_image_view = {};

		/** Texture sampler. */
		vk::Sampler m_vk_sampler = {};

		/** Filtering. */
		vk::Filter m_vk_filtering = {};

		/** Texture width. */
		uint32_t m_width = 0;

		/** Texture height. */
		uint32_t m_height = 0;
	};
}