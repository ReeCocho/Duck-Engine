#pragma once

/**
 * @file command_manager.hpp
 * @brief Command buffer and pool manager.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "vulkan_utilities.hpp"

namespace dk
{
	class VkCommandManager;

	/**
	 * @brief A command buffer created by a command manager.
	 */
	class VkManagedCommandBuffer
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Command manager.
		 * @param Logical device.
		 * @param Command buffer level.
		 * @param Thread created for.
		 */
		VkManagedCommandBuffer(VkCommandManager& command_manager, const vk::Device logical_device, vk::CommandBufferLevel level, size_t thread);

		/**
		 * @brief Destructor.
		 */
		~VkManagedCommandBuffer() = default;

		/**
		 * @brief Release the command buffer.
		 */
		void free();

		/**
		 * @brief Get command buffer.
		 * @return Command buffer.
		 */
		vk::CommandBuffer& get_command_buffer()
		{
			return m_vk_command_buffer;
		}

		/**
		 * @brief Get the command manager.
		 * @return The command manager.
		 */
		VkCommandManager& get_command_manager() const
		{
			return m_command_manager;
		}

		/**
		 * @brief Get command buffer level.
		 * @return Command buffer level.
		 */
		vk::CommandBufferLevel get_level() const
		{
			return m_vk_level;
		}

		/**
		 * @brief Get thread index the buffer was created for.
		 * @return Thread index.
		 */
		size_t get_thread_index() const
		{
			return m_thread;
		}

	private:

		/** Command manager. */
		VkCommandManager& m_command_manager;

		/** Logical device */
		const vk::Device& m_vk_logical_device;

		/** Command buffer. */
		vk::CommandBuffer m_vk_command_buffer;

		/** Level. */
		vk::CommandBufferLevel m_vk_level;

		/** Thread index. */
		size_t m_thread = 0;
	};



	/**
	 * @brief Command buffer and pool manager.
	 */
	class VkCommandManager
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Logical device.
		 * @param Queue family indices.
		 * @param Number of threads supported.
		 */
		VkCommandManager(const vk::Device& logical_device, QueueFamilyIndices qfi, size_t thread_count);

		/**
		 * @brief Destructor.
		 */
		~VkCommandManager();

		/**
		 * @brief Get pool count.
		 * @return Pool count.
		 */
		size_t pool_count() const
		{
			return m_vk_pools.size();
		}

		/**
		 * @brief Get nth pool.
		 * @param Pool index.
		 * @return Pool at index n.
		 */
		vk::CommandPool& get_pool(size_t n)
		{
			dk_assert(n < m_vk_pools.size());
			return m_vk_pools[n];
		}

		/**
		 * @brief Create a command buffer.
		 * @param Command buffer level.
		 */
		VkManagedCommandBuffer allocate_command_buffer(vk::CommandBufferLevel level);

	private:

		/** Logical device */
		const vk::Device& m_vk_logical_device;

		/** Command pools. */
		std::vector<vk::CommandPool> m_vk_pools;

		/** Index of the next pool to allocate a command buffer on. */
		size_t m_next_pool = 0;
	};
}