/**
 * @file resource_manager.cpp
 * @brief Resource manager source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "resource_manager.hpp"

namespace dk
{
	ResourceManager::ResourceManager(Renderer* renderer) : m_renderer(renderer)
	{
		m_mesh_allocator = std::make_unique<ResourceAllocator<Mesh>>(32);
		m_shader_allocator = std::make_unique<ResourceAllocator<Shader>>(8);
		m_material_allocator = std::make_unique<ResourceAllocator<Material>>(16);
		m_texture_allocator = std::make_unique<ResourceAllocator<Texture>>(8);
	}

	void ResourceManager::shutdown()
	{
		for(size_t i = 0; i < m_mesh_allocator->max_allocated(); ++i)
			if (m_mesh_allocator->is_allocated(i))
			{
				m_mesh_allocator->get_resource_by_handle(i)->free();
				m_mesh_allocator->deallocate(i);
			}

		for (size_t i = 0; i < m_shader_allocator->max_allocated(); ++i)
			if (m_shader_allocator->is_allocated(i))
			{
				m_shader_allocator->get_resource_by_handle(i)->free();
				m_shader_allocator->deallocate(i);
			}

		for (size_t i = 0; i < m_material_allocator->max_allocated(); ++i)
			if (m_material_allocator->is_allocated(i))
			{
				m_material_allocator->get_resource_by_handle(i)->free();
				m_material_allocator->deallocate(i);
			}

		for(size_t i = 0; i < m_texture_allocator->max_allocated(); ++i)
			if (m_texture_allocator->is_allocated(i))
			{
				m_texture_allocator->get_resource_by_handle(i)->free();
				m_texture_allocator->deallocate(i);
			}

		m_mesh_allocator.reset();
		m_shader_allocator.reset();
		m_material_allocator.reset();
		m_texture_allocator.reset();
	}

	Handle<Mesh> ResourceManager::create_mesh(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
	{
		if (m_mesh_allocator->num_allocated() + 1 > m_mesh_allocator->max_allocated())
			m_mesh_allocator->resize(m_mesh_allocator->max_allocated() + 16);

		auto mesh = Handle<Mesh>(m_mesh_allocator->allocate(), m_mesh_allocator.get());
		::new(m_mesh_allocator->get_resource_by_handle(mesh.id))(Mesh)(&m_renderer->get_graphics(), indices, vertices);
		
		return mesh;
	}

	Handle<Shader> ResourceManager::create_shader(const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code)
	{
		if (m_shader_allocator->num_allocated() + 1 > m_shader_allocator->max_allocated())
			m_shader_allocator->resize(m_shader_allocator->max_allocated() + 16);

		auto shader = Handle<Shader>(m_shader_allocator->allocate(), m_shader_allocator.get());
		::new(m_shader_allocator->get_resource_by_handle(shader.id))(Shader)
		(
			&m_renderer->get_graphics(),
			m_renderer->get_shader_render_pass(),
			vert_byte_code,
			frag_byte_code
		);

		return shader;
	}

	Handle<Material> ResourceManager::create_material(Handle<Shader> shader)
	{
		if (m_material_allocator->num_allocated() + 1 > m_material_allocator->max_allocated())
			m_material_allocator->resize(m_material_allocator->max_allocated() + 16);

		auto material = Handle<Material>(m_material_allocator->allocate(), m_material_allocator.get());
		::new(m_material_allocator->get_resource_by_handle(material.id))(Material)(&m_renderer->get_graphics(), shader);

		return material;
	}

	Handle<Texture> ResourceManager::create_texture(const std::string& path, vk::Filter filtering)
	{
		if (m_texture_allocator->num_allocated() + 1 > m_texture_allocator->max_allocated())
			m_texture_allocator->resize(m_texture_allocator->max_allocated() + 16);

		auto texture = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator.get());
		::new(m_texture_allocator->get_resource_by_handle(texture.id))(Texture)(&m_renderer->get_graphics(), path, filtering);

		return texture;
	}

	void ResourceManager::destroy(Handle<Mesh> mesh)
	{
		dk_assert(m_mesh_allocator->is_allocated(mesh.id));
		mesh->free();
		m_mesh_allocator->deallocate(mesh.id);
	}

	void ResourceManager::destroy(Handle<Shader> shader)
	{
		dk_assert(m_shader_allocator->is_allocated(shader.id));
		shader->free();
		m_shader_allocator->deallocate(shader.id);
	}

	void ResourceManager::destroy(Handle<Material> material)
	{
		dk_assert(m_material_allocator->is_allocated(material.id));
		material->free();
		m_material_allocator->deallocate(material.id);
	}

	void ResourceManager::destroy(Handle<Texture> texture)
	{
		dk_assert(m_texture_allocator->is_allocated(texture.id));
		texture->free();
		m_texture_allocator->deallocate(texture.id);
	}
}