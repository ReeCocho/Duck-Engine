/**
 * @file debugging.cpp
 * @brief Vulkan debugging source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "utilities\debugging.hpp"
#include "debugging.hpp"

namespace dk
{
	VkDebugger::VkDebugger(const vk::Instance& instance) : m_vk_instance(instance)
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;		// Debug flags
		createInfo.pfnCallback = debug_callback;												// Debug function

																								// Create debug function
		auto func = (PFN_vkCreateDebugReportCallbackEXT)m_vk_instance.getProcAddr("vkCreateDebugReportCallbackEXT");
		assert(func != nullptr);

		// Create debug callback
		VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
		func(static_cast<VkInstance>(m_vk_instance), &createInfo, nullptr, &callback);

		// Convert to C++ Vulkan
		m_vk_debug_callback = vk::DebugReportCallbackEXT(callback);
	}

	VkDebugger::~VkDebugger()
	{
		// Destroy debug callback
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)m_vk_instance.getProcAddr("vkDestroyDebugReportCallbackEXT");
		assert(func);
		func(static_cast<VkInstance>(m_vk_instance), static_cast<VkDebugReportCallbackEXT>(m_vk_debug_callback), nullptr);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugger::debug_callback
	(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData
	)
	{
		dk_log("Vulkan Validation Layer: " << msg);
		return VK_FALSE;
	}
}