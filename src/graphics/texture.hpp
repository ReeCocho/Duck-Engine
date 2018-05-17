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
		 * @param Mip map levels.
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
			uint32_t height,
			uint32_t mip_map_levels = 1
		);

		/**
		 * @brief Graphics context.
		 * @param Path to file containing image.
		 * @param Filtering.
		 * @param Mip map levels.
		 */
		Texture(Graphics* graphics, const std::string& path, vk::Filter filtering, uint32_t mip_map_levels = 1);

		/**
		 * @brief Destructor.
		 */
		virtual ~Texture() = default;

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
		 * Get mip map levels.
		 * @return Mip map levels.
		 */
		uint32_t get_mip_map_levels() const
		{
			return m_mip_map_levels;
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

	protected:

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

		/** Mip map levels. */
		uint32_t m_mip_map_levels = 1;

		/** Texture width. */
		uint32_t m_width = 0;

		/** Texture height. */
		uint32_t m_height = 0;
	};

	/**
	 * @brief 3D texture.
	 */
	class CubeMap : public Texture
	{
	public:

		/**
		 * Default constructor.
		 */
		CubeMap() = default;

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Image.
		 * @param Image view.
		 * @param Sampler.
		 * @param Image memory.
		 * @param Width.
		 * @param Height.
		 */
		CubeMap
		(
			Graphics* graphics, 
			vk::Image image, 
			vk::ImageView& imageView, 
			vk::Sampler sampler, 
			vk::DeviceMemory memory, 
			vk::Filter filter,
			uint32_t width, 
			uint32_t height
		);

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Path to file containing top image.
		 * @param Path to file containing bottom image.
		 * @param Path to file containing north image.
		 * @param Path to file containing east image.
		 * @param Path to file containing south image.
		 * @param Path to file containing west image.
		 * @param Texture filtering.
		 */
		CubeMap
		(
			Graphics* graphics, 
			const std::string& top, 
			const std::string& bottom,
			const std::string& north,
			const std::string& east,
			const std::string& south,
			const std::string& west,
			vk::Filter filter
		);
	};
}