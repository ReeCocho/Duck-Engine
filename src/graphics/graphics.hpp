#pragma once

/**
 * @file graphics.hpp
 * @brief Vulkan graphics context.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <string>
#include <memory>
#include <utilities\debugging.hpp>
#include "vulkan_utilities.hpp"
#include "device_manager.hpp"
#include "command_manager.hpp"
#include "debugging.hpp"

namespace dk
{
	/**
	 * @brief Vulkan graphics context.
	 */
	class Graphics
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Thread count.
		 * @param Window name.
		 * @param Window width.
		 * @param Window height.
		 */
		Graphics(size_t thread_count, const std::string& name, int width, int height);

		/**
		 * @brief Destructor.
		 */
		~Graphics();

		/**
		 * @brief Get the SDL window.
		 * @return The SDL window.
		 */
		const SDL_Window* get_window() const
		{
			return m_window;
		}

		/**
		 * @brief Get the Vulkan instance.
		 * @return The Vulkan instance.
		 */
		const vk::Instance& get_vk_instance() const
		{
			return m_vk_instance;
		}

		/**
		 * @brief Get the device manager.
		 * @return The device manager.
		 */
		VkDeviceManager& get_device_manager()
		{
			return *m_device_manager.get();
		}

		/**
		 * @brief Get the command manager.
		 * @return The command manager.
		 */
		VkCommandManager& get_command_manager()
		{
			return *m_command_manager.get();
		}

		/**
		 * @brief Get physical device.
		 * @return Physical device.
		 */
		const vk::PhysicalDevice& get_physical_device() const
		{
			return m_device_manager->get_physical_device();
		}

		/**
		 * @brief Get logical device.
		 * @return Logical device.
		 */
		const vk::Device& get_logical_device() const
		{
			return m_device_manager->get_logical_deivce();
		}

		/**
		 * @brief Get surface.
		 * @return Surface.
		 */
		const vk::SurfaceKHR& get_surface() const
		{
			return m_vk_surface;
		}

		/**
		 * @brief Get the name of the window.
		 * @return Name of the window.
		 */
		const std::string& get_name() const
		{
			return m_name;
		}

		/**
		 * @brief Get the width of the window.
		 * @return Window width.
		 */
		int get_width() const
		{
			int w, h;
			SDL_GetWindowSize(m_window, &w, &h);
			return w;
		}

		/**
		 * @brief Get the height of the window.
		 * @return Window height.
		 */
		int get_height() const
		{
			int w, h;
			SDL_GetWindowSize(m_window, &w, &h);
			return h;
		}

		/**
		 * @brief Create a Vulkan buffer and memory.
		 * @param Size of data.
		 * @param Usage.
		 * @param Memory properties
		 */
		VkMemBuffer create_buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

		/**
		 * @brief Copy memory contained in one buffer into another.
		 * @param Source buffer.
		 * @param Destination buffer.
		 * @param Size of data.
		 */
		void copy_buffer(const vk::Buffer& src, const vk::Buffer& dst, vk::DeviceSize size);

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other graphics context.
		 */
		Graphics(const Graphics& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other graphics context.
		 * @return This.
		 */
		Graphics& operator=(const Graphics& other) { return *this; };



		/** SDL window. */
		SDL_Window* m_window;

		/** Window name. */
		std::string m_name;

		/** Vulkan instance. */
		vk::Instance m_vk_instance;
		
		/** Vulkan surface to draw on. */
		vk::SurfaceKHR m_vk_surface;

		/** Debugger */
		std::unique_ptr<VkDebugger> m_debugger;

		/** Device manager. */
		std::unique_ptr<VkDeviceManager> m_device_manager;

		/** Command manager. */
		std::unique_ptr<VkCommandManager> m_command_manager;
	};
}