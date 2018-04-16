/**
 * @file graphics.cpp
 * @brief Vulkan graphics context source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <config.hpp>
#include "graphics.hpp"

namespace dk
{
	Graphics::Graphics(size_t thread_count, const std::string& name, int width, int height) :
		m_name(name)
	{
		// Bounds checking
		dk_assert(width > 0);
		dk_assert(height > 0);
		dk_assert(thread_count > 0);

		// Initialize SDL video
		if (SDL_Init(SDL_INIT_VIDEO | SDL_VIDEO_VULKAN) != 0)
			dk_err("SDL: Unable to initialize video.");

		// Create SDL window
		m_window = SDL_CreateWindow
		(
			m_name.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
		);

		// Check window creation
		dk_assert(m_window);

		// Get Vulkan extensions required by SDL
		const char** sdl_extensions = nullptr;
		uint32_t sdl_extension_count = 0;
		{
			auto check = SDL_Vulkan_GetInstanceExtensions(m_window, &sdl_extension_count, nullptr);
			dk_assert(check);
		}
		sdl_extensions = static_cast<const char**>(SDL_malloc(sizeof(const char*) * sdl_extension_count));
		{
			auto check = SDL_Vulkan_GetInstanceExtensions(m_window, &sdl_extension_count, sdl_extensions);
			dk_assert(check);
		}
		
		// Description of the application
		vk::ApplicationInfo appInfo = {};
		appInfo.pApplicationName = m_name.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Duck Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Check for Vulkan layers
		std::vector<const char*> layers = 
		{
#if DUCK_DEBUG_VULKAN
			"VK_LAYER_LUNARG_standard_validation"
#endif
		};
		dk_assert(layers.size() == get_layers(layers).size());

		// Check for Vulkan extensions
		std::vector<const char*> extensions =
		{
#ifdef DUCK_DEBUG_VULKAN
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
			VK_KHR_SURFACE_EXTENSION_NAME
		};

		// Add extensions required by SDL
		for (size_t i = 0; i < sdl_extension_count; ++i)
			extensions.push_back(sdl_extensions[i]);
		dk_assert(extensions.size() == get_extensions(extensions).size());
		
		// Create Vulkan instance
		vk::InstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		vk::Result result = vk::createInstance(&createInfo, nullptr, &m_vk_instance);
		dk_assert(result == vk::Result::eSuccess);

		// Free extension memory
		SDL_free(static_cast<void*>(sdl_extensions));

		// Create surface
		{
			// Create Vulkan surface.
			VkSurfaceKHR surface = VK_NULL_HANDLE;
			SDL_Vulkan_CreateSurface(m_window, static_cast<VkInstance>(m_vk_instance), &surface);

			dk_assert(surface != VK_NULL_HANDLE);

			m_vk_surface = static_cast<vk::SurfaceKHR>(surface);
		}

#if DUCK_DEBUG_VULKAN
		m_debugger = std::make_unique<VkDebugger>(m_vk_instance);
#endif

		// Create device manager
		const std::vector<const char*> device_extensions = 
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		m_device_manager = std::make_unique<VkDeviceManager>(m_vk_instance, m_vk_surface, layers, device_extensions);

		// Create command manager
		m_command_manager = std::make_unique<VkCommandManager>(get_logical_device(), m_device_manager->get_queue_family_indices(), thread_count);
	}

	void Graphics::shutdown()
	{
		// Wait for logical device to finish whatever it was doing
		m_device_manager->get_logical_deivce().waitIdle();

		// Destroy command manager
		m_command_manager.reset();

		// Destroy device manager
		m_device_manager.reset();

		// Destroy surface
		m_vk_instance.destroySurfaceKHR(m_vk_surface);

		// Destroy debugger
#if DUCK_DEBUG_VULKAN
		m_debugger.reset();
#endif

		// Destroy Vulkan instance
		m_vk_instance.destroy();

		// Cleanup window
		SDL_DestroyWindow(m_window);
	}

	VkMemBuffer Graphics::create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
	{
		VkMemBuffer buffer = {};
		
		auto qfi = m_device_manager->get_queue_family_indices();
		std::array<uint32_t, 2> queues = 
		{ 
			static_cast<uint32_t>(qfi.graphics_family), 
			static_cast<uint32_t>(qfi.transfer_family) 
		};

		// Create buffer
		vk::BufferCreateInfo buffer_info = {};
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = vk::SharingMode::eConcurrent;
		buffer_info.queueFamilyIndexCount = static_cast<uint32_t>(queues.size());
		buffer_info.pQueueFamilyIndices = queues.data();

		buffer.buffer = get_logical_device().createBuffer(buffer_info);
		dk_assert(buffer.buffer);

		// Allocate memory
		vk::MemoryRequirements mem_requirements = get_logical_device().getBufferMemoryRequirements(buffer.buffer);

		vk::MemoryAllocateInfo alloc_info = {};
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = find_memory_type(get_physical_device(), mem_requirements.memoryTypeBits, properties);

		buffer.memory = get_logical_device().allocateMemory(alloc_info);
		dk_assert(buffer.memory);
		
		// Bind buffer to memory
		get_logical_device().bindBufferMemory(buffer.buffer, buffer.memory, 0);
		return buffer;
	}

	void Graphics::copy_buffer(const vk::Buffer& src, const vk::Buffer& dst, vk::DeviceSize size)
	{
		// Create the command buffer used for the transfer
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = m_command_manager->get_transfer_pool();
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;

		vk::CommandBuffer command_buffer = get_logical_device().allocateCommandBuffers(alloc_info)[0];
		dk_assert(command_buffer);

		// Begin writing to the command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		command_buffer.begin(begin_info);

		// Copy data
		vk::BufferCopy copy_region = {};
		copy_region.size = size;
		command_buffer.copyBuffer(src, dst, copy_region);

		// End command buffer
		command_buffer.end();

		// Submit command buffer to transfer queue
		vk::SubmitInfo submit_info = {};
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		m_device_manager->get_transfer_queue().submit(submit_info, vk::Fence());
		m_device_manager->get_transfer_queue().waitIdle();

		// Free command buffer
		get_logical_device().freeCommandBuffers(m_command_manager->get_transfer_pool(), command_buffer);
	}

	void Graphics::create_image
	(
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& image_memory,
		vk::ImageCreateFlags flags,
		uint32_t array_layers
	)
	{
		vk::ImageCreateInfo image_info = {};
		image_info.setImageType(vk::ImageType::e2D);
		image_info.setExtent({ width, height, 1 });;
		image_info.setMipLevels(1);
		image_info.setArrayLayers(array_layers);
		image_info.setFormat(format);
		image_info.setTiling(tiling);
		image_info.setInitialLayout(vk::ImageLayout::eUndefined);
		image_info.setUsage(usage);
		image_info.setSamples(vk::SampleCountFlagBits::e1);
		image_info.setSharingMode(vk::SharingMode::eExclusive);
		image_info.setFlags(flags);

		// Create image
		{
			auto check = get_logical_device().createImage(&image_info, nullptr, &image);
			dk_assert(check == vk::Result::eSuccess);
		}

		vk::MemoryRequirements mem_requirements = get_logical_device().getImageMemoryRequirements(image);

		vk::MemoryAllocateInfo alloc_info = {};
		alloc_info.setAllocationSize(mem_requirements.size);
		alloc_info.setMemoryTypeIndex(find_memory_type(get_physical_device(), mem_requirements.memoryTypeBits, properties));

		// Allocate memory
		{
			auto check = get_logical_device().allocateMemory(&alloc_info, nullptr, &image_memory);
			dk_assert(check == vk::Result::eSuccess);
		}

		get_logical_device().bindImageMemory(image, image_memory, 0);
	}

	void Graphics::transition_image_layout(const vk::Image& image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout, uint32_t image_count)
	{
		vk::CommandBuffer command_buffer = begin_single_time_commands();

		vk::ImageMemoryBarrier barrier = {};
		barrier.setOldLayout(old_layout);
		barrier.setNewLayout(new_layout);
		barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.setImage(image);

		// Stage masks
		vk::PipelineStageFlags dstStageFlags;
		vk::PipelineStageFlags srcStageFlags;

		if (new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
		{
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

			if (format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint)
				barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
		else
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = image_count;

		if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = (vk::AccessFlagBits)0;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStageFlags = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			srcStageFlags = vk::PipelineStageFlagBits::eTransfer;
			dstStageFlags = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eVertexInput;
		}
		else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
		{
			barrier.srcAccessMask = (vk::AccessFlagBits)0;
			barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStageFlags = vk::PipelineStageFlagBits::eEarlyFragmentTests;
		}
		else
			dk_err("VULKAN: Unsupported layout transition");

		command_buffer.pipelineBarrier
		(
			srcStageFlags, dstStageFlags,
			(vk::DependencyFlagBits)0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		end_single_time_commands(command_buffer);
	}

	void Graphics::copy_buffer_to_image(const vk::Buffer& buffer, const vk::Image& image, uint32_t width, uint32_t height, uint32_t image_count, uint32_t image_size)
	{
		vk::CommandBuffer command_buffer = begin_single_time_commands();

		std::vector<vk::BufferImageCopy> regions(image_count);
		uint32_t offset = 0;

		for (size_t i = 0; i < regions.size(); ++i)
		{
			regions[i].setBufferOffset(offset);
			regions[i].setBufferRowLength(0);
			regions[i].setBufferImageHeight(0);
			regions[i].imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			regions[i].imageSubresource.mipLevel = 0;
			regions[i].imageSubresource.baseArrayLayer = static_cast<uint32_t>(i);
			regions[i].imageSubresource.layerCount = 1;
			regions[i].setImageOffset({ 0, 0, 0 });
			regions[i].setImageExtent({ width, height, 1 });
			offset += image_size;
		}

		// Copy buffer to image
		command_buffer.copyBufferToImage
		(
			buffer,
			image,
			vk::ImageLayout::eTransferDstOptimal,
			static_cast<uint32_t>(regions.size()),
			regions.data()
		);

		end_single_time_commands(command_buffer);
	}

	vk::ImageView Graphics::create_image_view(const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect_flags, vk::ImageViewType view_type, uint32_t image_count)
	{
		vk::ImageViewCreateInfo viewInfo = {};
		viewInfo.setImage(image);
		viewInfo.setViewType(view_type);
		viewInfo.setFormat(format);
		viewInfo.setComponents(vk::ComponentMapping());
		viewInfo.subresourceRange.aspectMask = aspect_flags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = image_count;

		vk::ImageView view;

		{
			auto check = get_logical_device().createImageView(&viewInfo, nullptr, &view);
			dk_assert(check == vk::Result::eSuccess);
		}

		return view;
	}

	vk::CommandBuffer Graphics::begin_single_time_commands()
	{
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.setLevel(vk::CommandBufferLevel::ePrimary);
		alloc_info.setCommandPool(m_command_manager->get_single_use_pool());
		alloc_info.setCommandBufferCount(1);

		auto command_buffer = get_logical_device().allocateCommandBuffers(alloc_info)[0];

		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		command_buffer.begin(begin_info);

		return command_buffer;
	}

	void Graphics::end_single_time_commands(vk::CommandBuffer command_buffer)
	{
		command_buffer.end();

		vk::SubmitInfo submitInfo = {};
		submitInfo.setCommandBufferCount(1);
		submitInfo.setPCommandBuffers(&command_buffer);

		m_device_manager->get_graphics_queue().submit(1, &submitInfo, { nullptr });
		m_device_manager->get_graphics_queue().waitIdle();

		get_logical_device().freeCommandBuffers(m_command_manager->get_single_use_pool(), 1, &command_buffer);
	}

	FrameBufferAttachment Graphics::create_attachment(vk::Format format, vk::ImageUsageFlags usage)
	{
		FrameBufferAttachment new_attachment = {};

		vk::ImageAspectFlags aspect_mask = (vk::ImageAspectFlagBits)0;
		vk::ImageLayout image_layout;

		new_attachment.format = format;

		// Get aspect mask and image layout
		if (usage & vk::ImageUsageFlagBits::eColorAttachment)
		{
			aspect_mask = vk::ImageAspectFlagBits::eColor;
			image_layout = vk::ImageLayout::eColorAttachmentOptimal;
		}
		if (usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
		{
			aspect_mask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
			image_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		}

		vk::ImageCreateInfo image = {};
		image.setImageType(vk::ImageType::e2D);
		image.setFormat(format);
		image.extent.width = get_width();
		image.extent.height = get_height();
		image.extent.depth = 1;
		image.setMipLevels(1);
		image.setArrayLayers(1);
		image.setSamples(vk::SampleCountFlagBits::e1);
		image.setTiling(vk::ImageTiling::eOptimal);
		image.setUsage(usage | vk::ImageUsageFlagBits::eSampled);

		// Create image
		new_attachment.image = get_logical_device().createImage(image);

		vk::MemoryAllocateInfo mem_alloc = {};
		vk::MemoryRequirements mem_reqs = get_logical_device().getImageMemoryRequirements(new_attachment.image);

		mem_alloc.allocationSize = mem_reqs.size;
		mem_alloc.memoryTypeIndex = find_memory_type(get_physical_device(), mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		// Allocate and bind image memory
		new_attachment.memory = get_logical_device().allocateMemory(mem_alloc);
		get_logical_device().bindImageMemory(new_attachment.image, new_attachment.memory, 0);

		new_attachment.view = create_image_view(new_attachment.image, format, aspect_mask);

		return new_attachment;
	}
}