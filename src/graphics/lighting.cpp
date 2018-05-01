/**
 * @file lighting.cpp
 * @brief Lighting source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "lighting.hpp"

namespace dk
{
	LightingManager::LightingManager(Graphics* graphics, size_t point_light_count, size_t dir_light_count) :
		m_graphics(graphics)
	{
		// Resize light vectors
		m_point_lights.resize(point_light_count);
		m_directional_lights.reserve(dir_light_count);

		// Create lighting buffer
		m_lighting_ubo = m_graphics->create_buffer
		(
			sizeof(m_lighting_data),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
		m_lighting_map = m_graphics->get_logical_device().mapMemory(m_lighting_ubo.memory, 0, sizeof(m_lighting_data));

		// Create SSBOs
		create_point_light_ssbo();
		create_directional_ssbo();
	}

	LightingManager::~LightingManager()
	{
		// Cleanup
		destroy_point_light_ssbo();
		destroy_directional_light_ssbo();
		m_graphics->get_logical_device().unmapMemory(m_lighting_ubo.memory);
		m_lighting_ubo.free(m_graphics->get_logical_device());
	}

	void LightingManager::upload()
	{
		// Upload lighting data
		memcpy(m_lighting_map, &m_lighting_data, sizeof(m_lighting_data));

		// Upload point light data
		memcpy(m_point_light_map, &m_point_light_count, sizeof(uint32_t));
		memcpy((void*)((int*)m_point_light_map + sizeof(uint32_t)), m_point_lights.data(), sizeof(PointLightData) * m_point_lights.size());

		// Upload directional light data
		memcpy(m_directional_light_map, &m_directional_light_count, sizeof(uint32_t));
		memcpy((void*)((int*)m_directional_light_map + sizeof(uint32_t)), m_directional_lights.data(), sizeof(DirectionalLightData) * m_directional_lights.size());
	}

	void LightingManager::flush_queues()
	{
		m_point_light_count = 0;
		m_directional_light_count = 0;
	}

	void LightingManager::draw(PointLightData data)
	{
		if (m_point_light_count + 1 > m_point_lights.size())
		{
			destroy_point_light_ssbo();
			m_point_lights.resize(m_point_lights.size() + 32);
			create_point_light_ssbo();
		}

		m_point_lights[m_point_light_count++] = data;
	}

	void LightingManager::draw(DirectionalLightData data)
	{
		if (m_directional_light_count + 1 > m_directional_lights.size())
		{
			destroy_directional_light_ssbo();
			m_directional_lights.resize(m_directional_lights.size() + 8);
			create_directional_ssbo();
		}

		m_directional_lights[m_directional_light_count++] = data;
	}

	void LightingManager::create_point_light_ssbo()
	{
		size_t data_size = 16 + (sizeof(PointLightData) * m_point_lights.size());

		m_point_light_ssbo = m_graphics->create_buffer
		(
			data_size,
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
		m_point_light_map = m_graphics->get_logical_device().mapMemory(m_point_light_ssbo.memory, 0, data_size);
	}

	void LightingManager::create_directional_ssbo()
	{
		size_t data_size = 16 + (sizeof(DirectionalLightData) * m_directional_lights.size());

		m_directional_light_ssbo = m_graphics->create_buffer
		(
			data_size,
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
		m_directional_light_map = m_graphics->get_logical_device().mapMemory(m_directional_light_ssbo.memory, 0, data_size);
	}

	void LightingManager::destroy_point_light_ssbo()
	{
		m_graphics->get_logical_device().unmapMemory(m_point_light_ssbo.memory);
		m_point_light_ssbo.free(m_graphics->get_logical_device());
	}

	void LightingManager::destroy_directional_light_ssbo()
	{
		m_graphics->get_logical_device().unmapMemory(m_directional_light_ssbo.memory);
		m_directional_light_ssbo.free(m_graphics->get_logical_device());
	}
}