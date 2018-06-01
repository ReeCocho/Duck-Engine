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
		m_archive(archive),
		m_writing(false),
		m_fields({}),
		m_name("")
	{
		dk_assert(archive);
	}

	ComponentArchive::ComponentArchive(Scene* scene, ResourceManager* resource_manager, bool writing) :
		m_scene(scene),
		m_resource_manager(resource_manager),
		m_archive(nullptr),
		m_writing(writing),
		m_fields({}),
		m_name("")
	{

	}

	bool ComponentArchive::field_exists(const std::string& name)
	{
		for (const auto& field : m_fields)
			if (field.name == name)
				return true;
		return false;
	}

	template<>
	void ComponentArchive::field<Handle<Mesh>>(const std::string& name, Handle<Mesh>* data)
	{
		if (m_archive)
		{
			// Set the field
			set_field<Handle<Mesh>>(name, data);

			// Mesh name
			std::string mesh_name;

			if (m_archive->is_writing())
			{
				// Write name to archive
				if(*data != Handle<Mesh>())
					mesh_name = m_resource_manager->get_mesh_name(*data);
				m_archive->write<std::string>(mesh_name);
			}
			else
			{
				// Read name the archive
				mesh_name = m_archive->read<std::string>();
				*data = m_resource_manager->get_mesh(mesh_name);
			}
		}
		else
		{
			if (m_writing)
				// Create the field
				set_field<Handle<Mesh>>(name, data);
			else
			{
				// Set the field if it exists
				Field field = get_field(name);
				if (field.data) *data = *static_cast<Handle<Mesh>*>(field.data);
			}
		}
	}

	template<>
	void ComponentArchive::field<Handle<MaterialShader>>(const std::string& name, Handle<MaterialShader>* data)
	{
		if (m_archive)
		{
			// Set the field
			set_field<Handle<MaterialShader>>(name, data);

			// Shader name
			std::string shader_name;

			if (m_archive->is_writing())
			{
				// Write name to archive
				if (*data != Handle<MaterialShader>())
					shader_name = m_resource_manager->get_shader_name(*data);
				m_archive->write<std::string>(shader_name);
			}
			else
			{
				// Read name the archive
				shader_name = m_archive->read<std::string>();
				*data = m_resource_manager->get_shader(shader_name);
			}
		}
		else
		{
			if (m_writing)
				// Create the field
				set_field<Handle<MaterialShader>>(name, data);
			else
			{
				// Set the field if it exists
				Field field = get_field(name);
				if (field.data) *data = *static_cast<Handle<MaterialShader>*>(field.data);
			}
		}
	}

	template<>
	void ComponentArchive::field<Handle<Material>>(const std::string& name, Handle<Material>* data)
	{
		if (m_archive)
		{
			// Set the field
			set_field<Handle<Material>>(name, data);

			// Material name
			std::string material_name;

			if (m_archive->is_writing())
			{
				// Write name to archive
				if (*data != Handle<Material>())
					material_name = m_resource_manager->get_material_name(*data);
				m_archive->write<std::string>(material_name);
			}
			else
			{
				// Read name the archive
				material_name = m_archive->read<std::string>();
				*data = m_resource_manager->get_material(material_name);
			}
		}
		else
		{
			if (m_writing)
				// Create the field
				set_field<Handle<Material>>(name, data);
			else
			{
				// Set the field if it exists
				Field field = get_field(name);
				if (field.data) *data = *static_cast<Handle<Material>*>(field.data);
			}
		}
	}

	template<>
	void ComponentArchive::field<Handle<SkyBox>>(const std::string& name, Handle<SkyBox>* data)
	{
		if (m_archive)
		{
			// Set the field
			set_field<Handle<SkyBox>>(name, data);

			// Sky box name
			std::string sky_box_name;

			if (m_archive->is_writing())
			{
				// Write name to archive
				if (*data != Handle<SkyBox>())
					sky_box_name = m_resource_manager->get_sky_box_name(*data);
				m_archive->write<std::string>(sky_box_name);
			}
			else
			{
				// Read name the archive
				sky_box_name = m_archive->read<std::string>();
				*data = m_resource_manager->get_sky_box(sky_box_name);
			}
		}
		else
		{
			if (m_writing)
				// Create the field
				set_field<Handle<SkyBox>>(name, data);
			else
			{
				// Set the field if it exists
				Field field = get_field(name);
				if (field.data) *data = *static_cast<Handle<SkyBox>*>(field.data);
			}
		}
	}

	template<>
	void ComponentArchive::field<Handle<CubeMap>>(const std::string& name, Handle<CubeMap>* data)
	{
		if (m_archive)
		{
			// Set the field
			set_field<Handle<CubeMap>>(name, data);

			// Cube map name
			std::string cube_map_name;

			if (m_archive->is_writing())
			{
				// Write name to archive
				if (*data != Handle<CubeMap>())
					cube_map_name = m_resource_manager->get_cube_map_name(*data);
				m_archive->write<std::string>(cube_map_name);
			}
			else
			{
				// Read name the archive
				cube_map_name = m_archive->read<std::string>();
				*data = m_resource_manager->get_cube_map(cube_map_name);
			}
		}
		else
		{
			if (m_writing)
				// Create the field
				set_field<Handle<CubeMap>>(name, data);
			else
			{
				// Set the field if it exists
				Field field = get_field(name);
				if (field.data) *data = *static_cast<Handle<CubeMap>*>(field.data);
			}
		}
	}
}