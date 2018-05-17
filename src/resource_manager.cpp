/**
 * @file resource_manager.cpp
 * @brief Resource manager source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <json.hpp>
#include <fstream>
#include <utilities\debugging.hpp>
#include <utilities\file_io.hpp>
#include "resource_manager.hpp"

/** For convenience */
using json = nlohmann::json;

namespace dk
{
	ResourceManager::ResourceManager(ForwardRenderer* renderer) :
		m_renderer(renderer),
		m_mesh_map({}),
		m_shader_map({}),
		m_material_map({}),
		m_texture_map({}),
		m_sky_box_map({}),
		m_cube_map_map({})
	{
		// Create allocators
		m_mesh_allocator = std::make_unique<ResourceAllocator<Mesh>>(32);
		m_shader_allocator = std::make_unique<ResourceAllocator<Shader>>(4);
		m_material_allocator = std::make_unique<ResourceAllocator<Material>>(8);
		m_texture_allocator = std::make_unique<ResourceAllocator<Texture>>(8);
		m_sky_box_allocator = std::make_unique<ResourceAllocator<SkyBox>>(1);
		m_cube_map_allocator = std::make_unique<ResourceAllocator<CubeMap>>(1);
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

		for (size_t i = 0; i < m_sky_box_allocator->max_allocated(); ++i)
			if (m_sky_box_allocator->is_allocated(i))
			{
				m_sky_box_allocator->get_resource_by_handle(i)->free();
				m_sky_box_allocator->deallocate(i);
			}

		for (size_t i = 0; i < m_cube_map_allocator->max_allocated(); ++i)
			if (m_cube_map_allocator->is_allocated(i))
			{
				m_cube_map_allocator->get_resource_by_handle(i)->free();
				m_cube_map_allocator->deallocate(i);
			}

		m_mesh_map.clear();
		m_shader_map.clear();
		m_material_map.clear();
		m_texture_map.clear();
		m_sky_box_map.clear();
		m_cube_map_map.clear();

		m_mesh_allocator.reset();
		m_shader_allocator.reset();
		m_material_allocator.reset();
		m_texture_allocator.reset();
		m_sky_box_allocator.reset();
		m_cube_map_allocator.reset();
	}

	void ResourceManager::load_resources
	(
		const std::string& meshes,
		const std::string& textures,
		const std::string& shaders,
		const std::string& materials,
		const std::string& cube_maps,
		const std::string& sky_boxes
	)
	{
		// Resource JSON
		json mesh_j;
		json shader_j;
		json material_j;
		json texture_j;
		json cube_map_j;
		json sky_box_j;

		// Load resource files
		{
			// Meshes
			std::ifstream stream(meshes + "resources.json");
			dk_assert(stream.is_open());
			stream >> mesh_j;
			stream.close();

			// Textures
			stream.open(textures + "resources.json");
			dk_assert(stream.is_open());
			stream >> texture_j;
			stream.close();

			// Shaders
			stream.open(shaders + "resources.json");
			dk_assert(stream.is_open());
			stream >> shader_j;
			stream.close();

			// Materials
			stream.open(materials + "resources.json");
			dk_assert(stream.is_open());
			stream >> material_j;
			stream.close();

			// Cube maps
			stream.open(cube_maps + "resources.json");
			dk_assert(stream.is_open());
			stream >> cube_map_j;
			stream.close();

			// Sky boxes
			stream.open(sky_boxes + "resources.json");
			dk_assert(stream.is_open());
			stream >> sky_box_j;
			stream.close();
		}

		// Load meshes
		for (const std::string& path : mesh_j["files"])
		{
			// Load mesh file
			std::ifstream stream(meshes + path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;
			stream.close();

			// Load file
			std::string mesh_path = j["path"];
			Handle<Mesh> mesh = create_mesh(path, meshes + mesh_path);

			// Calculate normals if requested
			if (j["calc_normals"]) mesh->compute_normals();
		}

		// Load textures
		for (const std::string& path : texture_j["files"])
		{
			// Load texture file
			std::ifstream stream(textures + path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;
			stream.close();

			// Find filtering mode
			vk::Filter filter = vk::Filter::eLinear;

			if (j["filter"] == "nearest")
				filter = vk::Filter::eNearest;
			else if (j["filter"] == "linear")
				filter = vk::Filter::eLinear;

			// Create texture
			std::string tex_path = j["path"];
			create_texture(path, textures + tex_path, filter);
		}

		// Load cube maps
		for (const std::string& path : cube_map_j["files"])
		{
			// Load cube map file
			std::ifstream stream(cube_maps + path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;
			stream.close();

			// Find filtering mode
			vk::Filter filter = vk::Filter::eLinear;

			if (j["filter"] == "nearest")
				filter = vk::Filter::eNearest;
			else if (j["filter"] == "linear")
				filter = vk::Filter::eLinear;

			// Create cube map
			std::string top_path = j["top"];
			std::string bottom_path = j["bottom"];
			std::string north_path = j["north"];
			std::string east_path = j["east"];
			std::string south_path = j["south"];
			std::string west_path = j["west"];
			create_cube_map
			(
				path,
				cube_maps + top_path,
				cube_maps + bottom_path,
				cube_maps + north_path,
				cube_maps + east_path,
				cube_maps + south_path,
				cube_maps + west_path,
				filter
			);
		}

		// Load shaders
		for (const std::string& path : shader_j["files"])
		{
			// Load shader file
			std::ifstream stream(shaders + path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;
			stream.close();

			// Create shader
			std::string vert_path = j["vertex"];
			std::string frag_path = j["fragment"];
			create_shader
			(
				path,
				read_binary_file(shaders + vert_path),
				read_binary_file(shaders + frag_path),
				j["depth"]
			);
		}

		// Load materials
		for (const std::string& path : material_j["files"])
		{
			// Load material file
			std::ifstream stream(materials + path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;
			stream.close();

			// Create material
			Handle<Material> material = create_material(path, get_shader(j["shader"]));

			// Set textures
			for (auto& texture : j["textures"])
				material->set_texture(texture["index"], get_texture(texture["path"]));

			// Set cube maps
			for (auto& cube_map : j["cubemaps"])
				material->set_cube_map(cube_map["index"], get_cube_map(cube_map["path"]));
		}

		// Load sky boxes
		for (const std::string& path : sky_box_j["files"])
		{
			// Load sky box file
			std::ifstream stream(sky_boxes + path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;
			stream.close();

			// Create sky box
			Handle<SkyBox> sky_box = create_sky_box(path);
			sky_box->set_material(get_material(j["material"]));
			sky_box->set_mesh(get_mesh(j["mesh"]));
		}
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

	Handle<Mesh> ResourceManager::create_mesh(const std::string& name, const std::string& path)
	{
		dk_assert(m_mesh_map.find(name) == m_mesh_map.end());

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

		m_mesh_map[name] = mesh.id;

		return mesh;
	}

	Handle<Shader> ResourceManager::create_shader(const std::string& name, const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code, bool depth)
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
			{ m_renderer->get_descriptor_set_layout() },
			vert_byte_code,
			frag_byte_code,
			depth
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

	Handle<Texture> ResourceManager::create_texture(const std::string& name, const std::string& path, vk::Filter filtering)
	{
		dk_assert(m_texture_map.find(name) == m_texture_map.end());

		if (m_texture_allocator->num_allocated() + 1 > m_texture_allocator->max_allocated())
			m_texture_allocator->resize(m_texture_allocator->max_allocated() + 16);

		auto texture = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator.get());
		::new(m_texture_allocator->get_resource_by_handle(texture.id))(Texture)(&m_renderer->get_graphics(), path, filtering);

		m_texture_map[name] = texture.id;

		return texture;
	}

	Handle<SkyBox> ResourceManager::create_sky_box(const std::string& name)
	{
		dk_assert(m_sky_box_map.find(name) == m_sky_box_map.end());

		if (m_sky_box_allocator->num_allocated() + 1 > m_sky_box_allocator->max_allocated())
			m_sky_box_allocator->resize(m_sky_box_allocator->max_allocated() + 2);

		auto sky_box = Handle<SkyBox>(m_sky_box_allocator->allocate(), m_sky_box_allocator.get());
		::new(m_sky_box_allocator->get_resource_by_handle(sky_box.id))(SkyBox)(&m_renderer->get_graphics());

		m_sky_box_map[name] = sky_box.id;

		return sky_box;
	}

	Handle<CubeMap> ResourceManager::create_cube_map
	(
		const std::string& name,
		const std::string& top,
		const std::string& bottom,
		const std::string& north,
		const std::string& east,
		const std::string& south,
		const std::string& west,
		vk::Filter filter
	)
	{
		dk_assert(m_cube_map_map.find(name) == m_cube_map_map.end());

		if (m_cube_map_allocator->num_allocated() + 1 > m_cube_map_allocator->max_allocated())
			m_cube_map_allocator->resize(m_cube_map_allocator->max_allocated() + 2);

		auto cube_map = Handle<CubeMap>(m_cube_map_allocator->allocate(), m_cube_map_allocator.get());
		::new(m_cube_map_allocator->get_resource_by_handle(cube_map.id))(CubeMap)
		(
			&m_renderer->get_graphics(),
			top, bottom,
			north, east,
			south, west,
			filter
		);

		m_cube_map_map[name] = cube_map.id;

		return cube_map;
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

	void ResourceManager::destroy(Handle<SkyBox> sky_box)
	{
		dk_assert(m_sky_box_allocator->is_allocated(sky_box.id));

		for (auto sky_box_id : m_sky_box_map)
			if (sky_box_id.second == sky_box.id)
			{
				m_sky_box_map.erase(sky_box_id.first);
				break;
			}

		sky_box->free();
		m_sky_box_allocator->deallocate(sky_box.id);
	}

	void ResourceManager::destroy(Handle<CubeMap> cube_map)
	{
		dk_assert(m_cube_map_allocator->is_allocated(cube_map.id));

		for (auto cube_map_id : m_cube_map_map)
			if (cube_map_id.second == cube_map.id)
			{
				m_cube_map_map.erase(cube_map_id.first);
				break;
			}

		cube_map->free();
		m_cube_map_allocator->deallocate(cube_map.id);
	}
}