/**
 * @file scene_util.cpp
 * @brief Scene utilities source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\hex.hpp>
#include "scene_util.hpp"

namespace dk
{
	/**
	 * Serialize a scene field in a JSON format.
	 * @param JSON element.
	 * @param Field.
	 * @param Resource manager for resource handles.
	 * @param Serializable scene for system names.
	 */
	static void serialize_scene_field_as_json
	(
		json::value_type& j, 
		ReflectionContext::Field& field, 
		ResourceManager& resource_manager,
		SerializableScene& scene
	)
	{
		// Field properties
		j["name"] = field.name;
		j["type"] = static_cast<uint64_t>(field.type);

		if(field.type == ReflectionContext::FieldType::Variable)
			j["data"] = binary_to_hex((char*)field.data, field.data_size);
		else if (field.type == ReflectionContext::FieldType::Vector)
		{
			auto& vec_field = static_cast<ReflectionContext::VectorField&>(field);
			for (size_t i = 0; i < vec_field.elements.size(); ++i)
				serialize_scene_field_as_json(j["data"][i], *vec_field.elements[i].get(), resource_manager, scene);
		}
		else if (field.type == ReflectionContext::FieldType::Handle)
		{
			auto& handle_field = static_cast<ReflectionContext::HandleField&>(field);
			
			if (handle_field.null_handle)
			{
				j["data"]["null_handle"] = true;
				j["data"]["system"] = "";
				j["data"]["id"] = 0;
			}
			else
			{
				j["data"]["null_handle"] = false;

				// Resources
				if (handle_field.resource_type == TypeID<Mesh>::id())
					j["data"]["name"] = resource_manager.get_mesh_name(*(dk::HMesh*)handle_field.data);
				else if (handle_field.resource_type == TypeID<MaterialShader>::id())
					j["data"]["name"] = resource_manager.get_shader_name(*(dk::HMaterialShader*)handle_field.data);
				else if (handle_field.resource_type == TypeID<Material>::id())
					j["data"]["name"] = resource_manager.get_material_name(*(dk::HMaterial*)handle_field.data);
				else if (handle_field.resource_type == TypeID<Texture>::id())
					j["data"]["name"] = resource_manager.get_texture_name(*(dk::HTexture*)handle_field.data);
				else if (handle_field.resource_type == TypeID<SkyBox>::id())
					j["data"]["name"] = resource_manager.get_sky_box_name(*(dk::HSkyBox*)handle_field.data);
				else if (handle_field.resource_type == TypeID<CubeMap>::id())
					j["data"]["name"] = resource_manager.get_cube_map_name(*(dk::HCubeMap*)handle_field.data);
				else
					// Component
					for (const auto& system : scene.systems)
						if (system.id == handle_field.resource_type)
						{
							j["data"]["system"] = system.name;
							j["data"]["id"] = handle_field.resource_id;
							break;
						}
			}
		}
		else if(field.type == ReflectionContext::FieldType::Enum)
			j["data"] = binary_to_hex((char*)field.data, field.data_size);
	}

	json serializable_scene_to_json(SerializableScene& scene, ResourceManager& resource_manager)
	{
		json j = {};

		// Scene properties
		j["entity_id_counter"] = scene.entity_id_counter;
		j["free_entity_ids"] = scene.free_entities;

		for (size_t i = 0; i < scene.systems.size(); ++i)
		{
			// System properties
			SerializableSystem& system = scene.systems[i];
			j["systems"][i]["name"] = system.name;

			for (size_t k = 0; k < system.components.size(); ++k)
			{
				// Component properties
				SerializableComponent& component = system.components[k];
				j["systems"][i]["components"][k]["entity"] = component.entity.get_id();
				j["systems"][i]["components"][k]["id"] = component.id;
				j["systems"][i]["components"][k]["name"] = component.reflection_context->get_name();
				
				// Serialize fields
				const auto& fields = component.reflection_context->get_fields();
				for (size_t l = 0; l < fields.size(); ++l)
					serialize_scene_field_as_json
					(
						j["systems"][i]["components"][k]["fields"][l], 
						*fields[l].get(), 
						resource_manager,
						scene
					);
			}
		}

		return j;
	}

	/**
	 * Deserialize a JSON field.
	 * @param Field to write to.
	 * @param JSON field.
	 * @param Scene we are deserializing to.
	 * @param Resource manager.
	 */
	static void deserialize_json_field(ReflectionContext::Field& field, json::value_type& j_field, Scene& scene, ResourceManager& resource_manager)
	{
		// Don't do anything if the types arn't the same
		if (j_field["type"] != field.type)
			return;

		if (field.type == ReflectionContext::FieldType::Variable || field.type == ReflectionContext::FieldType::Enum)
		{
			// Copy raw data
			auto bin_data = hex_to_binary(j_field["data"].get<std::string>());
			dk_assert(field.data_size <= bin_data.size());
			std::memcpy(field.data, bin_data.data(), field.data_size);
		}
		else if (field.type == ReflectionContext::FieldType::Vector)
		{
			auto& vec_field = static_cast<ReflectionContext::VectorField&>(field);
			vec_field.resize(j_field["data"].size());

			for (size_t i = 0; i < j_field["data"].size(); ++i)
				deserialize_json_field(*vec_field.get_element(i), j_field["data"][i], scene, resource_manager);
		}
		else if (field.type == ReflectionContext::FieldType::Handle)
		{
			auto& handle_field = static_cast<ReflectionContext::HandleField&>(field);

			if(!j_field["data"]["null_handle"])
			{
				// Resources
				if (handle_field.resource_type == TypeID<Mesh>::id())
					*(HMesh*)field.data = resource_manager.get_mesh(j_field["data"]["name"]);
				else if (handle_field.resource_type == TypeID<MaterialShader>::id())
					*(HMaterialShader*)field.data = resource_manager.get_shader(j_field["data"]["name"]);
				else if (handle_field.resource_type == TypeID<Material>::id())
					*(HMaterial*)field.data = resource_manager.get_material(j_field["data"]["name"]);
				else if (handle_field.resource_type == TypeID<Texture>::id())
					*(HTexture*)field.data = resource_manager.get_texture(j_field["data"]["name"]);
				else if (handle_field.resource_type == TypeID<SkyBox>::id())
					*(HSkyBox*)field.data = resource_manager.get_sky_box(j_field["data"]["name"]);
				else if (handle_field.resource_type == TypeID<CubeMap>::id())
					*(HCubeMap*)field.data = resource_manager.get_cube_map(j_field["data"]["name"]);
				// Component
				else
				{
					SystemBase* system = scene.get_system_by_name(j_field["data"]["system"]);

					// We can interpret the component as just a handle to any ol'
					// data type since it's just a pointer and an integer.
					*(Handle<char>*)field.data = Handle<char>(j_field["data"]["id"], (ResourceAllocator<char>*)system->get_component_allocator());
				}
			}
		}
	}

	void serializable_scene_from_json(json& j, Scene& scene, ResourceManager& resource_manager)
	{
		// Scene properties        
		scene.update_entities(j["entity_id_counter"], j["free_entity_ids"]);

		// Loop over every system
		for (size_t i = 0; i < j["systems"].size(); ++i)
		{
			SystemBase* system = scene.get_system_by_name(j["systems"][i]["name"].get<std::string>());
			dk_assert(system);
			
			// Loop over every component
			for (size_t k = 0; k < j["systems"][i]["components"].size(); ++k)
			{
				std::function<void(ComponentArchive&)> load_func = [&j, i, k, &scene, &resource_manager](ComponentArchive& archive)
				{
					const auto& fields = archive.get_fields();
					for (const auto& field : fields)
						for (size_t l = 0; l < j["systems"][i]["components"][k]["fields"].size(); ++l)
							if (j["systems"][i]["components"][k]["fields"][l]["name"].get<std::string>() == field->name)
								deserialize_json_field(*field.get(), j["systems"][i]["components"][k]["fields"][l], scene, resource_manager);
				};
				system->load_component
				(
					j["systems"][i]["components"][k]["id"],
					Entity(&scene, j["systems"][i]["components"][k]["entity"]),
					load_func
				);
			}
		}
	}
}