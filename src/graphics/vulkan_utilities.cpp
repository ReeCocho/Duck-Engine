#include <utilities\debugging.hpp>
#include <set>
#include "mesh.hpp"
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

	ShaderPipeline create_shader_pipeline(const vk::Device& logical_device, const ShaderPipelineCreateInfo& info)
	{
		ShaderPipeline pipeline = {};

		auto binding_description = Vertex::get_binding_description();
		auto attribute_descriptions = Vertex::get_attribute_descriptions();

		vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_info.pVertexBindingDescriptions = &binding_description;
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

		vk::PipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(info.width);
		viewport.height = static_cast<float>(info.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = vk::Extent2D(info.width, info.height);

		vk::PipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
												vk::ColorComponentFlagBits::eG |
												vk::ColorComponentFlagBits::eB |
												vk::ColorComponentFlagBits::eA;
		color_blend_attachment.blendEnable = VK_FALSE;
		color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne;
		color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero;
		color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
		color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;

		vk::PipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = vk::LogicOp::eCopy;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &color_blend_attachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		vk::PipelineDynamicStateCreateInfo dynamic_state = {};
		dynamic_state.dynamicStateCount = static_cast<uint32_t>(info.dynamic_states.size());
		dynamic_state.pDynamicStates = info.dynamic_states.data();

		vk::PipelineDepthStencilStateCreateInfo depth_stencil = {};
		depth_stencil.depthTestEnable = static_cast<vk::Bool32>(info.depth_test);
		depth_stencil.depthWriteEnable = static_cast<vk::Bool32>(info.depth_test) && static_cast<vk::Bool32>(info.depth_write);
		depth_stencil.depthCompareOp = info.depth_compare;
		depth_stencil.depthBoundsTestEnable = VK_FALSE;
		depth_stencil.minDepthBounds = 0.0f;
		depth_stencil.maxDepthBounds = 1.0f;
		depth_stencil.stencilTestEnable = VK_FALSE;
		depth_stencil.front = {};
		depth_stencil.back = {};

		// Create depth pipeline layout
		vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(info.descriptor_set_layouts.size());
		pipeline_layout_info.pSetLayouts = info.descriptor_set_layouts.data();
		pipeline_layout_info.pushConstantRangeCount = 0;
		pipeline_layout_info.pPushConstantRanges = nullptr;

		pipeline.layout = logical_device.createPipelineLayout(pipeline_layout_info);
		dk_assert(pipeline.layout);

		// Create depth pipeline
		vk::GraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.stageCount = static_cast<uint32_t>(info.pipeline_stages.size());
		pipeline_info.pStages = info.pipeline_stages.data();
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = &depth_stencil;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = pipeline.layout;
		pipeline_info.renderPass = info.render_pass;
		pipeline_info.subpass = 0;

		pipeline.pipeline = logical_device.createGraphicsPipeline(vk::PipelineCache(), pipeline_info);
		dk_assert(pipeline.pipeline);

		return pipeline;
	}
}