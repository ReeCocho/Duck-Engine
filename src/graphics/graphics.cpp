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
	Graphics::Graphics(const std::string& name, int width, int height) : m_name(name)
	{
		// Bounds checking
		dk_assert(width > 0);
		dk_assert(height > 0);

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
		layers = get_layers(layers);

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

		extensions = get_extensions(extensions);
		
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
		m_device_manager = std::make_unique<VkDeviceManager>(m_vk_instance, m_vk_surface, layers);
	}

	Graphics::~Graphics()
	{
		// Destroy device manager
		m_device_manager = nullptr;

		// Destroy surface
		m_vk_instance.destroySurfaceKHR(m_vk_surface);

		// Destroy debugger
#if DUCK_DEBUG_VULKAN
		m_debugger = nullptr;
#endif

		// Destroy Vulkan instance
		m_vk_instance.destroy();

		// Cleanup window
		SDL_DestroyWindow(m_window);
	}
}