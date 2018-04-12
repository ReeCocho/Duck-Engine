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
		m_shader(shader)
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
	}

	void Material::free()
	{
		m_vertex_uniform_buffer.free(m_graphics->get_logical_device());
		m_fragment_uniform_buffer.free(m_graphics->get_logical_device());
	}
}