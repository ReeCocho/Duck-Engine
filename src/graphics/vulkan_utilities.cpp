#include <utilities\debugging.hpp>
#include <set>
#include "vulkan_utilities.hpp"

namespace dk
{
	std::vector<const char*> get_layers(const std::vector<const char*>& layers)
	{
		// Get avaliable layers
		auto avaliable_layers = vk::enumerateInstanceLayerProperties();

		// Layers found
		std::vector<const char*> found_layers = {};

		// Loop over every layer requested
		for (auto layerName : layers)
			// Loop over every polled layer
			for (const auto& layerProperties : avaliable_layers)
				// Check if it's the requested layer
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					found_layers.push_back(layerName);
					break;
				}

		return found_layers;
	}

	std::vector<const char*> get_extensions(const std::vector<const char*>& extensions)
	{
		// Get installed extensions
		auto installedExtensions = vk::enumerateInstanceExtensionProperties();

		// Requested extensions
		std::vector<const char*> found_extensions = {};

		// Loop over every extension requested
		for (auto& extension : extensions)
			// Loop over every polled extension
			for (const auto& extension_property : installedExtensions)
				// Check if it's the requested extension
				if (strcmp(extension, extension_property.extensionName) == 0)
				{
					found_extensions.push_back(extension);
					break;
				}

		return found_extensions;
	}

	bool check_device_extension_support(const vk::PhysicalDevice& device, const std::vector<const char*>& extensions)
	{
		std::vector<vk::ExtensionProperties> available_extensions = device.enumerateDeviceExtensionProperties();
		std::set<std::string> required_extensions(extensions.begin(), extensions.end());

		for (const auto& extension : available_extensions)
			required_extensions.erase(extension.extensionName);

		return required_extensions.empty();
	}

	vk::PhysicalDevice find_suitable_physical_device(const vk::Instance& instance, const std::vector<const char*>& extensions, const vk::SurfaceKHR& surface)
	{
		// Current physical device
		vk::PhysicalDevice physical_device(static_cast<VkPhysicalDevice>(VK_NULL_HANDLE));

		// Device score
		uint32_t device_score = 0;

		// Find all devices
		std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
		dk_assert(devices.size() > 0);

		for (const auto& device : devices)
		{
			uint32_t current_device_score = 0;

			// Get device properties and features
			vk::PhysicalDeviceProperties properties = device.getProperties();
			vk::PhysicalDeviceFeatures features = device.getFeatures();

			// Device type
			switch (properties.deviceType)
			{
			case vk::PhysicalDeviceType::eDiscreteGpu:
				current_device_score += 1000;
				break;

			case vk::PhysicalDeviceType::eIntegratedGpu:
				current_device_score += 500;
				break;

			default:
				current_device_score += 100;
				break;
			}

			// Geometry shaders are cool
			if (features.geometryShader)
				current_device_score += 100;

			// Image dimension is a big factor
			current_device_score += properties.limits.maxImageDimension2D;

			// Make sure extensions are supported
			if (!check_device_extension_support(device, extensions))
				current_device_score = 0;
			else if (surface)
			{
				// Make sure swap chain is supported (If requested)
				SwapChainSupportDetails swap_chain_support = query_swap_chain_support(device, surface);
				if (swap_chain_support.formats.empty() || swap_chain_support.present_modes.empty())
					current_device_score = 0;
			}

			// Pick device
			if (current_device_score > device_score)
			{
				physical_device = device;
				device_score = current_device_score;
			}
		}

		return physical_device;
	}

	QueueFamilyIndices find_queue_family_indices(const vk::PhysicalDevice& physical_device, const vk::SurfaceKHR& surface)
	{
		QueueFamilyIndices qfi = {};

		// Get queue family properties of the device
		std::vector<vk::QueueFamilyProperties> properties = physical_device.getQueueFamilyProperties();

		int i = 0;
		for (const auto& queue_family : properties) 
		{
			// Get presentation support
			VkBool32 present_support = physical_device.getSurfaceSupportKHR(i, surface);

			if (queue_family.queueCount > 0)
			{
				// Find graphics family
				if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
					qfi.graphics_family = i;

				// Find transfer family
				if (!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics) && queue_family.queueFlags & vk::QueueFlagBits::eTransfer)
					qfi.transfer_family = i;

				// Find present family
				if (present_support)
					qfi.present_family = i;
			}

			if (qfi.is_complete() && qfi.graphics_family != qfi.present_family)
				break;

			++i;
		}

		return qfi;
	}

	SwapChainSupportDetails query_swap_chain_support(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
	{
		SwapChainSupportDetails details = {};
		details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
		details.formats = device.getSurfaceFormatsKHR(surface);
		details.present_modes = device.getSurfacePresentModesKHR(surface);
		return details;
	}

	vk::SurfaceFormatKHR choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& avalaible_formats)
	{
		if (avalaible_formats.size() == 1 && avalaible_formats[0].format == vk::Format::eUndefined)
			return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

		for (const auto& available_format : avalaible_formats)
			if (available_format.format == vk::Format::eB8G8R8A8Unorm && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return available_format;

		return avalaible_formats[0];
	}

	vk::PresentModeKHR choose_swap_present_mode(const std::vector<vk::PresentModeKHR>& presentation_modes, bool force_immediate)
	{
		// Fifo by default
		vk::PresentModeKHR best_mode = vk::PresentModeKHR::eFifo;

		// Prefer mailbox only if force_immediate is false. Otherwhise prefer immediate mode.
		for (const auto& present_mode : presentation_modes) 
		{
			if (present_mode == vk::PresentModeKHR::eMailbox) 
			{
				if (force_immediate)
					best_mode = present_mode;
				else
					return present_mode;
			}
			else if (present_mode == vk::PresentModeKHR::eImmediate) 
			{
				if (force_immediate) return present_mode;
				best_mode = present_mode;
			}
		}

		return best_mode;
	}

	vk::Extent2D choose_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		else 
		{
			vk::Extent2D actual_extent = { width, height };

			actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
			actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

			return actual_extent;
		}
	}

	vk::ShaderModule create_shader_module(const vk::Device& logical_device, const std::vector<char>& byte_code)
	{
		vk::ShaderModuleCreateInfo create_info = {};
		create_info.codeSize = byte_code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(byte_code.data());

		vk::ShaderModule shader_module = logical_device.createShaderModule(create_info);
		dk_assert(shader_module);

		return shader_module;
	}

	uint32_t find_memory_type(const vk::PhysicalDevice& physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties mem_properties = physical_device.getMemoryProperties();

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
			if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		throw std::runtime_error("Vulkan: Failed to find suitable memory type!");
	}

	vk::Format find_best_depth_format(const vk::PhysicalDevice& physical_device)
	{
		// Find a suitable depth format to use, starting with the best format
		std::vector<vk::Format> depthFormats =
		{
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD32Sfloat,
			vk::Format::eD24UnormS8Uint,
			vk::Format::eD16UnormS8Uint,
			vk::Format::eD16Unorm
		};

		for (const auto format : depthFormats)
		{
			const auto depthFormatProperties = physical_device.getFormatProperties(format);

			// Format must support depth stencil attachment for optimal tiling
			if (depthFormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
				return format;
		}

		return vk::Format::eD16Unorm;
	}
}