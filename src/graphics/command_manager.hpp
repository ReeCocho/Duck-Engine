#pragma once

/**
 * @file get_command_manager().hpp
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
		VkManagedCommandBuffer(VkCommandManager& command_manager, vk::Device& logical_device, vk::CommandBufferLevel level, size_t thread);

		/**
		 * @brief Destructor.
		 */
		~VkManagedCommandBuffer() = default;

		/**
		 * @brief Release the command buffer.
		 */
		void free();

		/**
		 * @brief Reset the command buffer.
		 */
		void reset();

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
		vk::Device& m_vk_logical_device;

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
		 * @brief Default constructor.
		 */
		VkCommandManager();

		/**
		 * @brief Constructor.
		 * @param Logical device.
		 * @param Queue family indices.
		 * @param Number of threads supported.
		 */
		VkCommandManager(vk::Device& logical_device, QueueFamilyIndices qfi, size_t thread_count);

		/**
		 * @brief Destructor.
		 */
		~VkCommandManager();

		/**
		 * @brief Get pool count.
		 * @return Pool count.
		 */
		size_t get_pool_count() const
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
		 * @brief Get transfer pool.
		 * @return Transfer pool.
		 */
		vk::CommandPool& get_transfer_pool()
		{
			return m_vk_transfer_pool;
		}

		/**
		 * @brief Create a command buffer.
		 * @param Command buffer level.
		 */
		VkManagedCommandBuffer allocate_command_buffer(vk::CommandBufferLevel level);

		/**
		 * @brief Get the logical device.
		 * @return The logical device.
		 */
		const vk::Device& get_logical_device() const
		{
			return m_vk_logical_device;
		}

	private:

		/** Logical device */
		vk::Device& m_vk_logical_device;

		/** Graphics command pools. */
		std::vector<vk::CommandPool> m_vk_pools;

		/** Transfer command pool. */
		vk::CommandPool m_vk_transfer_pool;

		/** Index of the next pool to allocate a command buffer on. */
		size_t m_next_pool = 0;
	};
}