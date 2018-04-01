#pragma once

/**
 * @file debugging.hpp
 * @brief Vulkan debugging class.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vulkan_utilities.hpp"

namespace dk
{
	/**
	 * @brief Vulkan debugger.
	 */
	class VkDebugger
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Instance to debug.
		 */
		VkDebugger(const vk::Instance& instance);

		/**
		 * @brief Destructor.
		 */
		~VkDebugger();

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other debugger.
		 */
		VkDebugger(const VkDebugger& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other debugger.
		 * @return This.
		 */
		VkDebugger& operator=(const VkDebugger& other) { return *this; }

		/**
		 * @brief Vulkan debugging callback.
		 */
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback
		(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData
		);

		/** Debug reporting callback. */
		vk::DebugReportCallbackEXT m_vk_debug_callback;

		/** Vulkan instance. */
		const vk::Instance& m_vk_instance;
	};
}