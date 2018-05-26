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
		 * @brief Default constructor.
		 */
		Graphics() = default;

		/**
		 * @brief Constructor.
		 * @param Thread count.
		 * @param Window name.
		 * @param Window width.
		 * @param Window height.
		 * @param Extra window flags.
		 */
		Graphics(size_t thread_count, const std::string& name, int width, int height, uint32_t flags = 0);

		/**
		 * @brief Destructor.
		 */
		~Graphics() = default;

		/**
		 * @brief Shutdown the graphics context.
		 */
		void shutdown();

		/**
		 * @brief Get the SDL window.
		 * @return The SDL window.
		 */
		SDL_Window* get_window() const
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
		vk::PhysicalDevice& get_physical_device()
		{
			return m_device_manager->get_physical_device();
		}

		/**
		 * @brief Get logical device.
		 * @return Logical device.
		 */
		vk::Device& get_logical_device()
		{
			return m_device_manager->get_logical_deivce();
		}

		/**
		 * @brief Get surface.
		 * @return Surface.
		 */
		vk::SurfaceKHR& get_surface()
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

		/**
		 * @brief Create image.
		 * @param Image width.
		 * @param Image height.
		 * @param Format.
		 * @param Tiling.
		 * @param Usage.
		 * @param Memory property flags.
		 * @param Image reference.
		 * @param Image memory reference.
		 * @param Number of images.
		 * @param Mip map levels.
		 */
		void create_image
		(
			uint32_t width,
			uint32_t height,
			vk::Format format,
			vk::ImageTiling tiling,
			vk::ImageUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::Image& image,
			vk::DeviceMemory& image_memory,
			vk::ImageCreateFlags flags = static_cast<vk::ImageCreateFlagBits>(0),
			uint32_t array_layers = 1,
			uint32_t mip_levels = 1
		);

		/**
		 * @brief Create transition image layout.
		 * @param Image to create transition layout for.
		 * @param Transition format.
		 * @param Old image layout.
		 * @param New image layout.
		 * @param Number of images.
		 * @param Mip map level.
		 * @param Image count.
		 */
		void transition_image_layout
		(
			const vk::Image& image, 
			vk::Format format, 
			vk::ImageLayout old_layout, 
			vk::ImageLayout new_layout,
			uint32_t image_count = 1,
			uint32_t mip_levels = 1
		);

		/**
		 * @brief Copy a buffer to an image.
		 * @param Buffer to copy from.
		 * @param Image to copy to.
		 * @param Image width.
		 * @param Image height.
		 * @param Number of images to copy.
		 * @param Size of each image.
		 * @note The image size can be zero if you only have one image to copy.
		 */
		void copy_buffer_to_image
		(
			const vk::Buffer& buffer, 
			const vk::Image& image, 
			uint32_t width, 
			uint32_t height,
			uint32_t image_count = 1,
			uint32_t image_size = 0
		);

		/**
		 * @brief Create image view.
		 * @param Image to create view for.
		 * @param Image view format.
		 * @param Image aspect flags.
		 * @param Image view type.
		 * @param Number of images.
		 * @param Mip levels.
		 * @return New image view.
		 */
		vk::ImageView create_image_view
		(
			const vk::Image& image, 
			vk::Format format, 
			vk::ImageAspectFlags aspect_flags,
			vk::ImageViewType view_type = vk::ImageViewType::e2D,
			uint32_t image_count = 1,
			uint32_t mip_levels = 1
		);

		/**
		 * @brief Create a single use command buffer.
		 * @return Command buffer.
		 */
		vk::CommandBuffer begin_single_time_commands();

		/**
		 * @brief Destroy a single use command buffer.
		 * @param Command buffer to destroy.
		 */
		void end_single_time_commands(vk::CommandBuffer command_buffer);

		/**
		 * @brief Create a framebuffer attachment.
		 * @param Framebuffer format.
		 * @param Usage.
		 * @return Framebuffer attachment.
		 */
		FrameBufferAttachment create_attachment(vk::Format format, vk::ImageUsageFlags usage);

		/**
		 * Generate mip maps for an image.
		 * @param Image.
		 * @param Width.
		 * @param Height.
		 * @param Mip map levels.
		 */
		void generate_mipmaps(const vk::Image& image, int32_t tex_width, int32_t tex_height, uint32_t mip_levels);

	private:

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