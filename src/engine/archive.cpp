/**
 * @file archive.cpp
 * @brief Archive source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "archive.hpp"

namespace dk
{
	ComponentArchive::ComponentArchive(Scene* scene, ResourceManager* resource_manager, Archive* archive) :
		m_scene(scene),
		m_resource_manager(resource_manager),
		m_archive(archive)
	{
		dk_assert(archive);
	}

	template<>
	void ComponentArchive::field<Mesh>(HMesh& data)
	{
		// Mesh name
		std::string mesh_name = "";

		if (m_archive->is_writing())
		{
			// Write name to archive
			if(data != HMesh())
				mesh_name = m_resource_manager->get_mesh_name(data);
			m_archive->write<std::string>(mesh_name);
		}
		else
		{
			// Read name the archive
			mesh_name = m_archive->read<std::string>();
			data = m_resource_manager->get_mesh(mesh_name);
		}
	}

	template<>
	void ComponentArchive::field<MaterialShader>(HMaterialShader& data)
	{
		// Shader name
		std::string shader_name = "";

		if (m_archive->is_writing())
		{
			// Write name to archive
			if (data != HMaterialShader())
				shader_name = m_resource_manager->get_shader_name(data);
			m_archive->write<std::string>(shader_name);
		}
		else
		{
			// Read name the archive
			shader_name = m_archive->read<std::string>();
			data = m_resource_manager->get_shader(shader_name);
		}
	}

	template<>
	void ComponentArchive::field<Material>(HMaterial& data)
	{
		// Material name
		std::string material_name = "";

		if (m_archive->is_writing())
		{
			// Write name to archive
			if (data != HMaterial())
				material_name = m_resource_manager->get_material_name(data);
			m_archive->write<std::string>(material_name);
		}
		else
		{
			// Read name the archive
			material_name = m_archive->read<std::string>();
			data = m_resource_manager->get_material(material_name);
		}
	}

	template<>
	void ComponentArchive::field<SkyBox>(HSkyBox& data)
	{
		// Sky box name
		std::string sky_box_name = "";

		if (m_archive->is_writing())
		{
			// Write name to archive
			if (data != HSkyBox())
				sky_box_name = m_resource_manager->get_sky_box_name(data);
			m_archive->write<std::string>(sky_box_name);
		}
		else
		{
			// Read name the archive
			sky_box_name = m_archive->read<std::string>();
			data = m_resource_manager->get_sky_box(sky_box_name);
		}
	}

	template<>
	void ComponentArchive::field<CubeMap>(HCubeMap& data)
	{
		// Cube map name
		std::string cube_map_name;

		if (m_archive->is_writing())
		{
			// Write name to archive
			if (data != HCubeMap())
				cube_map_name = m_resource_manager->get_cube_map_name(data);
			m_archive->write<std::string>(cube_map_name);
		}
		else
		{
			// Read name the archive
			cube_map_name = m_archive->read<std::string>();
			data = m_resource_manager->get_cube_map(cube_map_name);
		}
	}
}