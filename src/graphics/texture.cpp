/**
 * @file texture.cpp
 * @brief Textures source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "texture.hpp"

namespace dk
{
	Texture::Texture
	(
		Graphics* graphics,
		vk::Image image,
		vk::ImageView image_view,
		vk::Sampler sampler,
		vk::DeviceMemory memory,
		vk::Filter filter,
		uint32_t width,
		uint32_t height
	) : 
		m_graphics(graphics),
		m_vk_image(image),
		m_vk_image_view(image_view),
		m_vk_sampler(sampler),
		m_vk_memory(memory),
		m_vk_filtering(filter),
		m_width(width),
		m_height(height)
	{

	}

	Texture::Texture(Graphics* graphics, const std::string& path, vk::Filter filtering) : 
		m_graphics(graphics),
		m_vk_filtering(filtering)
	{
		auto logical_device = m_graphics->get_logical_device();

		// Load image from file
		int tex_width = 0, tex_height = 0, tex_channels = 0;
		auto pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

		m_width = static_cast<uint32_t>(tex_width);
		m_height = static_cast<uint32_t>(tex_height);

		dk_assert(pixels);

		auto image_size = static_cast<vk::DeviceSize>(m_width * m_height * 4);

		// Create staging buffer
		VkMemBuffer staging_buffer = m_graphics->create_buffer
		(
			image_size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		// Map image to memory
		void* data;
		logical_device.mapMemory(staging_buffer.memory, 0, image_size, (vk::MemoryMapFlagBits)0, &data);
		memcpy(data, pixels, static_cast<size_t>(image_size));
		logical_device.unmapMemory(staging_buffer.memory);

		// Free pixel data
		stbi_image_free(pixels);

		// Create image
		m_graphics->create_image
		(
			m_width,
			m_height,
			vk::Format::eR8G8B8A8Unorm,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			m_vk_image,
			m_vk_memory
		);

		// Prepare texture for shader access
		m_graphics->transition_image_layout(m_vk_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		m_graphics->copy_buffer_to_image(staging_buffer.buffer, m_vk_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
		m_graphics->transition_image_layout(m_vk_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		// Cleanup staging buffer
		logical_device.destroyBuffer(staging_buffer.buffer);
		logical_device.freeMemory(staging_buffer.memory);

		// Create texture image view
		m_vk_image_view = m_graphics->create_image_view(m_vk_image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);

		// Sampler creation info
		vk::SamplerCreateInfo samplerInfo = {};
		samplerInfo.setMagFilter(filtering);
		samplerInfo.setMinFilter(filtering);
		samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
		samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
		samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
		samplerInfo.setAnisotropyEnable(false);
		samplerInfo.setMaxAnisotropy(1);
		samplerInfo.setBorderColor(vk::BorderColor::eFloatOpaqueBlack);
		samplerInfo.setUnnormalizedCoordinates(false);
		samplerInfo.setCompareEnable(false);
		samplerInfo.setCompareOp(vk::CompareOp::eAlways);
		samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
		samplerInfo.setMipLodBias(0.0f);
		samplerInfo.setMinLod(0.0f);
		samplerInfo.setMaxLod(0.0f);

		// Create sampler
		m_vk_sampler = logical_device.createSampler(samplerInfo);
	}

	void Texture::free()
	{
		if (m_graphics)
		{
			auto logical_device = m_graphics->get_logical_device();
			if (m_vk_sampler) logical_device.destroySampler(m_vk_sampler);
			if (m_vk_image_view) logical_device.destroyImageView(m_vk_image_view);
			if (m_vk_memory) logical_device.freeMemory(m_vk_memory);
			if (m_vk_image) logical_device.destroyImage(m_vk_image);
		}
	}

	CubeMap::CubeMap
	(
		Graphics* graphics,
		vk::Image image,
		vk::ImageView& image_view,
		vk::Sampler sampler,
		vk::DeviceMemory memory,
		vk::Filter filter,
		uint32_t width,
		uint32_t height
	) : Texture(graphics, image, image_view, sampler, memory, filter, width, height) {}

	CubeMap::CubeMap
	(
		Graphics* graphics,
		const std::string& top,
		const std::string& bottom,
		const std::string& north,
		const std::string& east,
		const std::string& south,
		const std::string& west,
		vk::Filter filter
	)
	{
		m_graphics = graphics;
		m_vk_filtering = filter;

		auto& logical_device = m_graphics->get_logical_device();

		// Load image from file
		int tex_width = 0, tex_height = 0, tex_channels = 0;
		unsigned char* top_pixels		= stbi_load(top.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		unsigned char* bottom_pixels	= stbi_load(bottom.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		unsigned char* north_pixels		= stbi_load(north.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		unsigned char* east_pixels		= stbi_load(east.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		unsigned char* south_pixels		= stbi_load(south.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		unsigned char* west_pixels		= stbi_load(west.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

		m_width = static_cast<uint32_t>(tex_width);
		m_height = static_cast<uint32_t>(tex_height);
	
		dk_assert(top_pixels && bottom_pixels && north_pixels && east_pixels && south_pixels && west_pixels);
	
		auto image_size = static_cast<vk::DeviceSize>((m_width * m_height * 4) * 6);
		auto single_image_size = static_cast<vk::DeviceSize>(m_width * m_height * 4);

		// Combined images
		unsigned char* total = new unsigned char[static_cast<size_t>(image_size)];
		memcpy(total,							west_pixels, single_image_size);
		memcpy(total + single_image_size,		east_pixels, single_image_size);
		memcpy(total + (2 * single_image_size), top_pixels, single_image_size);
		memcpy(total + (3 * single_image_size), bottom_pixels, single_image_size);
		memcpy(total + (4 * single_image_size), north_pixels, single_image_size);
		memcpy(total + (5 * single_image_size), south_pixels, single_image_size);

		// Create staging buffer
		VkMemBuffer staging_buffer = m_graphics->create_buffer
		(
			image_size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
	
		// Map image to memory
		void* data;
		logical_device.mapMemory(staging_buffer.memory, 0, image_size, (vk::MemoryMapFlagBits)0, &data);
		memcpy(data, total, static_cast<size_t>(image_size));
		logical_device.unmapMemory(staging_buffer.memory);
	
		// Free pixel data
		stbi_image_free(top_pixels);
		stbi_image_free(bottom_pixels);
		stbi_image_free(north_pixels);
		stbi_image_free(east_pixels);
		stbi_image_free(south_pixels);
		stbi_image_free(west_pixels);
		delete total;
	
		// Create image
		m_graphics->create_image
		(
			m_width,
			m_height,
			vk::Format::eR8G8B8A8Unorm,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			m_vk_image,
			m_vk_memory,
			vk::ImageCreateFlagBits::eCubeCompatible,
			6
		);
	
		// Prepare texture for shader access
		m_graphics->transition_image_layout(m_vk_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 6);
		m_graphics->copy_buffer_to_image(staging_buffer.buffer, m_vk_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 6, static_cast<uint32_t>(single_image_size));
		m_graphics->transition_image_layout(m_vk_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 6);
	
		// Cleanup staging buffer
		logical_device.destroyBuffer(staging_buffer.buffer);
		logical_device.freeMemory(staging_buffer.memory);
	
		// Create texture image view
		m_vk_image_view = m_graphics->create_image_view(m_vk_image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::eCube, 6);
	
		// Sampler creation info
		vk::SamplerCreateInfo samplerInfo = {};
		samplerInfo.setMagFilter(filter);
		samplerInfo.setMinFilter(filter);
		samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
		samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
		samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
		samplerInfo.setAnisotropyEnable(false);
		samplerInfo.setMaxAnisotropy(1);
		samplerInfo.setBorderColor(vk::BorderColor::eFloatOpaqueBlack);
		samplerInfo.setUnnormalizedCoordinates(false);
		samplerInfo.setCompareEnable(false);
		samplerInfo.setCompareOp(vk::CompareOp::eAlways);
		samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
		samplerInfo.setMipLodBias(0.0f);
		samplerInfo.setMinLod(0.0f);
		samplerInfo.setMaxLod(0.0f);
	
		// Create sampler
		m_vk_sampler = logical_device.createSampler(samplerInfo);
	}
}