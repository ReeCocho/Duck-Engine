#pragma once

/**
 * @file vulkan_utilities.hpp
 * @brief Common utilities for Vulkan.
 * @author Connor J. Bramham (ReeCocho)
 */

#define SDL_MAIN_HANDLED

/** Includes. */
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>
#include <vulkan\vulkan.hpp>
#include <vector>

#undef max
#undef min

namespace dk
{
	/**
	 * Shader pipeline and pipeline layout.
	 */
	struct ShaderPipeline
	{
		/** Pipeline layout */
		vk::PipelineLayout layout;

		/** Pipeline. */
		vk::Pipeline pipeline;
	};

	/**
	 * Shader pipeline create info.
	 */
	struct ShaderPipelineCreateInfo
	{
		/** Render pass. */
		vk::RenderPass render_pass = {};

		/** Descriptor set layouts. */
		std::vector<vk::DescriptorSetLayout> descriptor_set_layouts = {};

		/** Push constants. */
		std::vector<vk::PushConstantRange> push_constant_ranges = {};

		/** Pipeline stages. */
		std::vector<vk::PipelineShaderStageCreateInfo> pipeline_stages = {};

		/** Vertex input information. */
		vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};

		/** How to assemble the vertex input. */
		vk::PipelineInputAssemblyStateCreateInfo input_assembly = {};

		/** Viewport description. */
		vk::PipelineViewportStateCreateInfo viewport_state = {};

		/** How to rasterize pixels. */
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};

		/** Multisampling. */
		vk::PipelineMultisampleStateCreateInfo multisampling = {};

		/** How to blend colors. */
		vk::PipelineColorBlendStateCreateInfo color_blending = {};

		/** Dynamic state. */
		vk::PipelineDynamicStateCreateInfo dynamic_state = {};

		/** Depth stenciling. */
		vk::PipelineDepthStencilStateCreateInfo depth_stencil = {};
	};

	/**
	 * @brief Framebuffer for offscreen rendering.
	 */
	struct FrameBufferAttachment 
	{
		/** Image. */
		vk::Image image = {};

		/** Image memory. */
		vk::DeviceMemory memory = {};
		
		/** Image view. */
		vk::ImageView view = {};

		/** Image format. */
		vk::Format format;
	};

	/**
	 * @brief Vulkan device queue family indices.
	 */
	struct QueueFamilyIndices 
	{
		int graphics_family = -1;

		int present_family = -1;

		int transfer_family = -1;

		/**
		 * @brief Check if all queues are avaliable.
		 * @return IF all queues are avaliable.
		 */
		bool is_complete() 
		{
			return graphics_family >= 0 && present_family >= 0 && transfer_family >= 0;
		}
	};

	/**
	 * @brief Information needed for swap chain creation.
	 */
	struct SwapChainSupportDetails 
	{
		/** Surface capabilities. */
		vk::SurfaceCapabilitiesKHR capabilities = {};

		/** Color space formats. */
		std::vector<vk::SurfaceFormatKHR> formats = {};

		/** Presentation modes. */
		std::vector<vk::PresentModeKHR> present_modes = {};
	};

	/**
	 * @brief Vulkan buffer with memory.
	 */
	struct VkMemBuffer
	{
		/** Buffer. */
		vk::Buffer buffer;

		/** Memory. */
		vk::DeviceMemory memory;

		/**
		 * @brief Free both the buffer and the memory.
		 * @param Logical device.
		 */
		void free(const vk::Device& logical_device)
		{
			logical_device.destroyBuffer(buffer);
			logical_device.freeMemory(memory);
		}
	};



	/**
	 * @brief Get a list of enabled layers.
	 * @param Requested layers
	 * @return Layers found.
	 */
	std::vector<const char*> get_layers(const std::vector<const char*>& layers);


	/**
	 * @brief Get a list of enabled extensions.
	 * @param Requested extensions
	 * @return Extensions found.
	 */
	std::vector<const char*> get_extensions(const std::vector<const char*>& extensions);

	/**
	 * @brief Check device extension support.
	 * @param Physical device.
	 * @param Device extensions.
	 * @return If the device supports every extension.
	 */
	bool check_device_extension_support(const vk::PhysicalDevice& device, const std::vector<const char*>& extensions);

	/**
	 * @brief Find a suitable physical device.
	 * @param Vulkan instance.
	 * @param Extensions required.
	 * @param Optional surface for swapchain support.
	 * @return The best physical device found.
	 * @note If no device was found, this function will return a VK_NULL_HANDLE.
	 */
	vk::PhysicalDevice find_suitable_physical_device(const vk::Instance& instance, const std::vector<const char*>& extensions = {}, const vk::SurfaceKHR& surface = {});

	/**
	 * @brief Find a physical devices queue family indices.
	 * @param Physical device.
	 * @param Surface.
	 * @return Devices queue family indices.
	 */
	QueueFamilyIndices find_queue_family_indices(const vk::PhysicalDevice& physical_device, const vk::SurfaceKHR& surface);

	/**
	 * @brief Get swapchain support details from a physical device.
	 * @param Physical device to check.
	 * @param Surface to check details of.
	 * @return Swap chain support details.
	 */
	SwapChainSupportDetails query_swap_chain_support(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);

	/**
	 * @brief Choose an appropriate surface format from avaliable formats.
	 * @param Avaliable formats.
	 * @return Best format.
	 */
	vk::SurfaceFormatKHR choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& avalaible_formats);

	/**
	 * @brief Choose an appropriate presentation mode.
	 * @param Available presentation modes.
	 * @param Force immediate mode.
	 */
	vk::PresentModeKHR choose_swap_present_mode(const std::vector<vk::PresentModeKHR>& presentation_modes, bool force_immediate = false);

	/**
	 * @brief Choose an appropriate swap chain image extent.
	 * @param Capabilities of the swap chain in question.
	 * @param Prefered width.
	 * @param Prefered height.
	 */
	vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

	/**
	 * @brief Create a shader module.
	 * @param Logical device.
	 * @param Shader byte code.
	 * @return Shader module.
	 */
	vk::ShaderModule create_shader_module(const vk::Device& logical_device, const std::vector<char>& byte_code);

	/**
	 * @brief Find an appropriate memory type.
	 * @param Physical device.
	 * @param Filter for types we want.
	 * @param Required properties.
	 */
	uint32_t find_memory_type(const vk::PhysicalDevice& physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties);

	/**
	 * @brief Find the best depth format.
	 * @param Physical device.
	 * @return Best depth format.
	 */
	vk::Format find_best_depth_format(const vk::PhysicalDevice& physical_device);

	/**
	 * Create a shader pipeline.
	 * @param Logical device.
	 * @param Shader create info.
	 * @return Shader pipeline.
	 */
	ShaderPipeline create_shader_pipeline(const vk::Device& logical_device, const ShaderPipelineCreateInfo& info);
}