/**
 * @file archive.cpp
 * @brief Archive source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "archive.hpp"

namespace dk
{
	SceneWriteArchive::SceneWriteArchive(Scene* scene, ResourceManager* resource_manager, size_t chunk_size) :
		m_scene(scene),
		m_resource_manager(resource_manager),
		m_chunk_size(chunk_size),
		m_systems_components({})
	{
		// Allocate default chunk
		m_data = new char[m_chunk_size];
		m_data_length = m_chunk_size;
		m_write_head = m_data;
	}

	void SceneWriteArchive::serialize()
	{
		// Resize system components
		m_systems_components.resize(m_scene->get_system_count());

		// Find system components
		for (size_t i = 0; i < m_scene->get_system_count(); ++i)
			m_systems_components[i] = m_scene->get_system_by_index(i)->get_active_components();

		// Write components
		for (size_t i = 0; i < m_scene->get_system_count(); ++i)
		{
			// Write component count
			write<uint32_t>(static_cast<uint32_t>(m_systems_components[i].size()));


		}
	}

	SceneWriteArchive::~SceneWriteArchive()
	{
		// Free data
		delete[] m_data;
	}

	void SceneWriteArchive::expand_archive()
	{
		// Allocate new memory
		void* new_data = new char[m_data_length + m_chunk_size];

		// Copy old memory
		std::memcpy(new_data, m_data, m_data_length);

		// Move write head
		m_write_head = (void*)((intptr_t)new_data + ((intptr_t)m_write_head - (intptr_t)m_data));

		// Increase length
		m_data_length += m_chunk_size;

		// Delete old memory
		delete[] m_data;
		m_data = new_data;
	}

	template<>
	void SceneWriteArchive::write<std::string>(std::string data)
	{
		// Expand archive if we need to
		while ((data.size() + 1 + (intptr_t)m_write_head) - (intptr_t)m_data > m_data_length)
			expand_archive();

		// Copy data
		std::memcpy(m_write_head, data.data(), data.size() + 1);

		// Move write head
		m_write_head = (void*)((intptr_t)m_write_head + data.size() + 1);
	}

	template<>
	void SceneWriteArchive::write<Entity>(Entity data)
	{
		write<uint32_t>(static_cast<uint32_t>(data.get_id()));
	}

	template<>
	void SceneWriteArchive::write<Handle<Mesh>>(Handle<Mesh> data)
	{
		// Get string name of the mesh
		std::string name = m_resource_manager->get_mesh_name(data);
		write<std::string>(name);
	}

	template<>
	void SceneWriteArchive::write<Handle<MaterialShader>>(Handle<MaterialShader> data)
	{
		// Get string name of the shader
		std::string name = m_resource_manager->get_shader_name(data);
		write<std::string>(name);
	}

	template<>
	void SceneWriteArchive::write<Handle<Material>>(Handle<Material> data)
	{
		// Get string name of the material
		std::string name = m_resource_manager->get_material_name(data);
		write<std::string>(name);
	}

	template<>
	void SceneWriteArchive::write<Handle<Texture>>(Handle<Texture> data)
	{
		// Get string name of the texture
		std::string name = m_resource_manager->get_texture_name(data);
		write<std::string>(name);
	}

	template<>
	void SceneWriteArchive::write<Handle<SkyBox>>(Handle<SkyBox> data)
	{
		// Get string name of the sky box
		std::string name = m_resource_manager->get_sky_box_name(data);
		write<std::string>(name);
	}

	template<>
	void SceneWriteArchive::write<Handle<CubeMap>>(Handle<CubeMap> data)
	{
		// Get string name of the cube map
		std::string name = m_resource_manager->get_cube_map_name(data);
		write<std::string>(name);
	}
}