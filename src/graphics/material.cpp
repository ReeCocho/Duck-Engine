/**
 * @file material.cpp
 * @brief Material source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "material.hpp"

namespace dk
{
	Material::Material() {}

	Material::Material(Graphics* graphics, Handle<Shader> shader) :
		m_graphics(graphics),
		m_shader(shader),
		m_textures({}),
		m_cube_maps({})
	{
		// Create buffers
		m_vertex_uniform_buffer = m_graphics->create_buffer
		(
			shader->get_vertex_buffer_size(), 
			vk::BufferUsageFlagBits::eUniformBuffer, 
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		m_fragment_uniform_buffer = m_graphics->create_buffer
		(
			shader->get_fragment_buffer_size(),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		// Create descriptor pool and descriptor set
		if (m_shader->get_texture_count() > 0)
		{
			vk::DescriptorPoolSize pool_size = {};
			pool_size.type = vk::DescriptorType::eCombinedImageSampler;
			pool_size.descriptorCount = static_cast<uint32_t>(m_shader->get_texture_count());

			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			pool_info.maxSets = 1;

			m_vk_descriptor_pool = m_graphics->get_logical_device().createDescriptorPool(pool_info);

			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_vk_descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_shader->get_texture_descriptor_set_layout();
			m_vk_texture_descriptor_set = m_graphics->get_logical_device().allocateDescriptorSets(alloc_info)[0];
		}

		// Create maps
		m_vertex_map = m_graphics->get_logical_device().mapMemory(m_vertex_uniform_buffer.memory, 0, m_shader->get_vertex_buffer_size());
		m_fragment_map = m_graphics->get_logical_device().mapMemory(m_fragment_uniform_buffer.memory, 0, m_shader->get_fragment_buffer_size());
	}

	Material::~Material() {}

	void Material::set_texture(size_t index, Handle<Texture> texture) 
	{
		dk_assert(index < m_shader->get_texture_count());
		m_cube_maps.erase(index);
		m_textures[index] = texture;
		update_texture_descriptor_set();
	}

	void Material::set_cube_map(size_t index, Handle<CubeMap> cube_map)
	{
		dk_assert(index < m_shader->get_texture_count());
		m_textures.erase(index);
		m_cube_maps[index] = cube_map;
		update_texture_descriptor_set();
	}

	void Material::free()
	{
		m_textures.clear();
		m_cube_maps.clear();
		m_graphics->get_logical_device().unmapMemory(m_vertex_uniform_buffer.memory);
		m_graphics->get_logical_device().unmapMemory(m_fragment_uniform_buffer.memory);
		m_graphics->get_logical_device().destroyDescriptorPool(m_vk_descriptor_pool);
		m_vertex_uniform_buffer.free(m_graphics->get_logical_device());
		m_fragment_uniform_buffer.free(m_graphics->get_logical_device());
	}

	void Material::update_texture_descriptor_set()
	{
		if (m_textures.size() + m_cube_maps.size() < m_shader->get_texture_count())
			return;

		std::vector<vk::WriteDescriptorSet> write_sets(m_shader->get_texture_count());
		std::vector<vk::DescriptorImageInfo> descriptor_image_info(m_shader->get_texture_count());

		// Textures
		for (auto texture : m_textures)
		{
			size_t i = texture.first;

			descriptor_image_info[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			descriptor_image_info[i].imageView = texture.second->get_image_view();
			descriptor_image_info[i].sampler = texture.second->get_sampler();

			write_sets[i].dstSet = m_vk_texture_descriptor_set;
			write_sets[i].dstBinding = static_cast<uint32_t>(i);
			write_sets[i].dstArrayElement = 0;
			write_sets[i].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			write_sets[i].descriptorCount = 1;
			write_sets[i].pImageInfo = &descriptor_image_info[i];
		}

		// Cube maps
		for (auto cube_map : m_cube_maps)
		{
			size_t i = cube_map.first;

			descriptor_image_info[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			descriptor_image_info[i].imageView = cube_map.second->get_image_view();
			descriptor_image_info[i].sampler = cube_map.second->get_sampler();

			write_sets[i].dstSet = m_vk_texture_descriptor_set;
			write_sets[i].dstBinding = static_cast<uint32_t>(i);
			write_sets[i].dstArrayElement = 0;
			write_sets[i].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			write_sets[i].descriptorCount = 1;
			write_sets[i].pImageInfo = &descriptor_image_info[i];
		}

		// Update descriptor sets
		m_graphics->get_logical_device().updateDescriptorSets(static_cast<uint32_t>(write_sets.size()), write_sets.data(), 0, nullptr);
	}
}