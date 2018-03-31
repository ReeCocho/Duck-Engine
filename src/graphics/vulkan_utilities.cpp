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
}