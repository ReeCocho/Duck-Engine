#pragma once

/**
 * @file scene_util.hpp
 * @brief Scene utilities header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <json.hpp>
#include <ecs\scene.hpp>
#include "resource_manager.hpp"

/** For convenience */
using json = nlohmann::json;

namespace dk
{
	/**
	 * Serialize a scene in a JSON format.
	 * @param Serializable scene.
	 * @param Resource manager for resource handles.
	 * @return JSON data.
	 */
	extern json scene_to_json(SerializableScene& scene, ResourceManager& resource_manager);

	/**
	 * Deserialize a scene from JSON.
	 * @param Scene to write.
	 * @param JSON data.
	 * @param Resource manager for resource handles.
	 */
	extern void scene_from_json(Scene& scene, json& j, ResourceManager& resource_manager);
}