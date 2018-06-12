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
	 * @param Serialzable scene.
	 * @param Resource manager for resource handles.
	 * @return JSON data.
	 */
	extern json serializable_scene_to_json(SerializableScene& scene, ResourceManager& resource_manager);

	/**
	 * Serialize a scene from JSON.
	 * @param JSON data.
	 * @param Scene to load to.
	 * @param Resource manager for resource handles.
	 */
	extern void serializable_scene_from_json(json& j, Scene& scene, ResourceManager& resource_manager);
}