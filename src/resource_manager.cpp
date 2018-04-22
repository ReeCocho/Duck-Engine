/**
 * @file resource_manager.cpp
 * @brief Resource manager source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "resource_manager.hpp"

namespace dk
{
	ResourceManager::ResourceManager(Renderer* renderer) : 
		m_renderer(renderer),
		m_mesh_map({}),
		m_shader_map({}),
		m_material_map({}),
		m_texture_map({})
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

		m_mesh_map.clear();
		m_shader_map.clear();
		m_material_map.clear();
		m_texture_map.clear();

		m_mesh_allocator.reset();
		m_shader_allocator.reset();
		m_material_allocator.reset();
		m_texture_allocator.reset();
	}

	Handle<Mesh> ResourceManager::create_mesh(const std::string& name, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
	{
		dk_assert(m_mesh_map.find(name) == m_mesh_map.end());

		if (m_mesh_allocator->num_allocated() + 1 > m_mesh_allocator->max_allocated())
			m_mesh_allocator->resize(m_mesh_allocator->max_allocated() + 16);

		auto mesh = Handle<Mesh>(m_mesh_allocator->allocate(), m_mesh_allocator.get());
		::new(m_mesh_allocator->get_resource_by_handle(mesh.id))(Mesh)(&m_renderer->get_graphics(), indices, vertices);
		
		m_mesh_map[name] = mesh.id;

		return mesh;
	}

	Handle<Mesh> ResourceManager::create_mesh(const std::string& path)
	{
		dk_assert(m_mesh_map.find(path) == m_mesh_map.end());

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
	
		// Load OBJ
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
			dk_err(err);
	
		std::vector<uint16_t> indices = {};
	
		std::vector<glm::vec3> vertices = {};
		std::vector<glm::vec2> uvs = {};
		std::vector<glm::vec3> normals = {};
	
		std::vector<Vertex> unique_vertices = {};
	
		for (const auto& shape : shapes)
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex = {};
	
				// Get position
				vertex.position =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
	
				// Get UV
				if (attrib.texcoords.size() > 0)
					vertex.uv =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
	
				// Get normal
				if (attrib.normals.size() > 0)
					vertex.normal =
				{
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
	
				// Check if duplicate vertex
				bool isUnique = true;
	
				for (size_t i = 0; i < unique_vertices.size(); ++i)
					if (unique_vertices[i] == vertex)
					{
						indices.push_back(static_cast<uint16_t>(i));
						isUnique = false;
						break;
					}
	
				if (isUnique)
				{
					indices.push_back(static_cast<uint16_t>(vertices.size()));
					vertices.push_back(vertex.position);
					uvs.push_back(vertex.uv);
					normals.push_back(vertex.normal);
					unique_vertices.push_back(vertex);
				}
			}

		if (m_mesh_allocator->num_allocated() + 1 > m_mesh_allocator->max_allocated())
			m_mesh_allocator->resize(m_mesh_allocator->max_allocated() + 16);

		auto mesh = Handle<Mesh>(m_mesh_allocator->allocate(), m_mesh_allocator.get());
		::new(m_mesh_allocator->get_resource_by_handle(mesh.id))(Mesh)(&m_renderer->get_graphics(), indices, unique_vertices);

		m_mesh_map[path] = mesh.id;

		return mesh;
	}

	Handle<Shader> ResourceManager::create_shader(const std::string& name, const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code)
	{
		dk_assert(m_shader_map.find(name) == m_shader_map.end());

		if (m_shader_allocator->num_allocated() + 1 > m_shader_allocator->max_allocated())
			m_shader_allocator->resize(m_shader_allocator->max_allocated() + 16);

		auto shader = Handle<Shader>(m_shader_allocator->allocate(), m_shader_allocator.get());
		::new(m_shader_allocator->get_resource_by_handle(shader.id))(Shader)
		(
			&m_renderer->get_graphics(),
			m_renderer->get_shader_render_pass(),
			m_renderer->get_depth_prepass(),
			vert_byte_code,
			frag_byte_code
		);

		m_shader_map[name] = shader.id;

		return shader;
	}

	Handle<Material> ResourceManager::create_material(const std::string& name, Handle<Shader> shader)
	{
		dk_assert(m_material_map.find(name) == m_material_map.end());

		if (m_material_allocator->num_allocated() + 1 > m_material_allocator->max_allocated())
			m_material_allocator->resize(m_material_allocator->max_allocated() + 16);

		auto material = Handle<Material>(m_material_allocator->allocate(), m_material_allocator.get());
		::new(m_material_allocator->get_resource_by_handle(material.id))(Material)(&m_renderer->get_graphics(), shader);

		m_material_map[name] = material.id;

		return material;
	}

	Handle<Texture> ResourceManager::create_texture(const std::string& path, vk::Filter filtering)
	{
		dk_assert(m_texture_map.find(path) == m_texture_map.end());

		if (m_texture_allocator->num_allocated() + 1 > m_texture_allocator->max_allocated())
			m_texture_allocator->resize(m_texture_allocator->max_allocated() + 16);

		auto texture = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator.get());
		::new(m_texture_allocator->get_resource_by_handle(texture.id))(Texture)(&m_renderer->get_graphics(), path, filtering);

		m_texture_map[path] = texture.id;

		return texture;
	}

	void ResourceManager::destroy(Handle<Mesh> mesh)
	{
		dk_assert(m_mesh_allocator->is_allocated(mesh.id));

		for(auto mesh_id : m_mesh_map)
			if (mesh_id.second == mesh.id)
			{
				m_mesh_map.erase(mesh_id.first);
				break;
			}

		mesh->free();
		m_mesh_allocator->deallocate(mesh.id);
	}

	void ResourceManager::destroy(Handle<Shader> shader)
	{
		dk_assert(m_shader_allocator->is_allocated(shader.id));

		for (auto shader_id : m_shader_map)
			if (shader_id.second == shader.id)
			{
				m_shader_map.erase(shader_id.first);
				break;
			}

		shader->free();
		m_shader_allocator->deallocate(shader.id);
	}

	void ResourceManager::destroy(Handle<Material> material)
	{
		dk_assert(m_material_allocator->is_allocated(material.id));

		for (auto material_id : m_material_map)
			if (material_id.second == material.id)
			{
				m_material_map.erase(material_id.first);
				break;
			}

		material->free();
		m_material_allocator->deallocate(material.id);
	}

	void ResourceManager::destroy(Handle<Texture> texture)
	{
		dk_assert(m_texture_allocator->is_allocated(texture.id));

		for (auto texture_id : m_texture_map)
			if (texture_id.second == texture.id)
			{
				m_texture_map.erase(texture_id.first);
				break;
			}

		texture->free();
		m_texture_allocator->deallocate(texture.id);
	}
}