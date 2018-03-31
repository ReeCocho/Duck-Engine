/**
 * @file graphics.cpp
 * @brief Vulkan graphics context source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
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

		// Create Vulkan instance
		vk::ApplicationInfo appInfo = {};
		appInfo.pApplicationName = m_name.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Duck Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		vk::InstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;

		vk::Result result = vk::createInstance(&createInfo, nullptr, &m_vk_instance);
		dk_assert(result == vk::Result::eSuccess);
	}

	Graphics::~Graphics()
	{
		// Destroy Vulkan instance
		m_vk_instance.destroy();

		// Cleanup window
		SDL_DestroyWindow(m_window);
	}
}