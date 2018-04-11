/**
 * @file get_command_manager().cpp
 * @brief Command manager source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\debugging.hpp>
#include "command_manager.hpp"

namespace dk
{
	VkManagedCommandBuffer::VkManagedCommandBuffer() : 
		m_command_manager(nullptr),
		m_vk_logical_device(vk::Device()),
		m_vk_level(vk::CommandBufferLevel::ePrimary),
		m_thread(0)
	{

	}

	VkManagedCommandBuffer::VkManagedCommandBuffer(VkCommandManager* command_manager, vk::Device& logical_device, vk::CommandBufferLevel level, size_t thread) :
		m_command_manager(command_manager),
		m_vk_logical_device(logical_device),
		m_vk_level(level),
		m_thread(thread)
	{
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = m_command_manager->get_pool(m_thread);
		alloc_info.level = level;
		alloc_info.commandBufferCount = 1;

		m_vk_command_buffer = m_vk_logical_device.allocateCommandBuffers(alloc_info)[0];
		dk_assert(m_vk_command_buffer);
	}

	void VkManagedCommandBuffer::free()
	{
		m_command_manager->get_logical_device().freeCommandBuffers(m_command_manager->get_pool(m_thread), m_vk_command_buffer);
	}

	void VkManagedCommandBuffer::reset()
	{
		m_vk_command_buffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
	}



	VkCommandManager::VkCommandManager() : m_vk_logical_device(vk::Device())
	{

	}

	VkCommandManager::VkCommandManager(vk::Device& logical_device, QueueFamilyIndices qfi, size_t thread_count) : m_vk_logical_device(logical_device)
	{
		m_vk_pools = std::vector<vk::CommandPool>(thread_count);

		// Create command pools
		for (auto& pool : m_vk_pools)
		{
			vk::CommandPoolCreateInfo pool_info = {};
			pool_info.queueFamilyIndex = qfi.graphics_family;
			pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

			pool = m_vk_logical_device.createCommandPool(pool_info);
			dk_assert(pool);
		}

		vk::CommandPoolCreateInfo pool_info = {};
		pool_info.queueFamilyIndex = qfi.transfer_family;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_vk_transfer_pool = m_vk_logical_device.createCommandPool(pool_info);
		dk_assert(m_vk_transfer_pool);
	}

	VkCommandManager::~VkCommandManager()
	{
		// Destroy pools
		for (auto& pool : m_vk_pools)
			if (pool)
			{
				m_vk_logical_device.destroyCommandPool(pool);
				pool = vk::CommandPool();
			}

		if (m_vk_transfer_pool)
		{
			m_vk_logical_device.destroyCommandPool(m_vk_transfer_pool);
			m_vk_transfer_pool = vk::CommandPool();
		}
	}

	VkManagedCommandBuffer& VkManagedCommandBuffer::operator=(VkManagedCommandBuffer& other)
	{
		m_command_manager = other.m_command_manager;
		m_thread = other.m_thread;
		m_vk_command_buffer = other.m_vk_command_buffer;
		m_vk_level = other.m_vk_level;
		m_vk_logical_device = other.m_vk_logical_device;
		return *this;
	}

	VkManagedCommandBuffer VkCommandManager::allocate_command_buffer(vk::CommandBufferLevel level)
	{
		auto cb = VkManagedCommandBuffer(this, m_vk_logical_device, level, m_next_pool);
		m_next_pool = (m_next_pool + 1) % m_vk_pools.size();
		return cb;
	}
}