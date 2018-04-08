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
}