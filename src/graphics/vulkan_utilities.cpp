#include <utilities\debugging.hpp>
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

	vk::PhysicalDevice find_suitable_physical_device(const vk::Instance& instance)
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

			// Pick device
			if (current_device_score > device_score)
			{
				physical_device = device;
				current_device_score = device_score;
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

			// Find graphics family
			if (queue_family.queueCount > 0 && queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
				qfi.graphics_family = i;

			// Find present family
			if (queue_family.queueCount > 0 && present_support)
				qfi.present_family = i;

			if (qfi.is_complete())
				break;

			++i;
		}

		return qfi;
	}
}