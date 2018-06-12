/**
 * @file lights.cpp
 * @brief Lights source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "transform.hpp"
#include <engine\config.hpp>
#include "lights.hpp"
#include <engine\common.hpp>

namespace dk
{
	void DirectionalLightSystem::on_begin()
	{
		Handle<DirectionalLight> light = get_component();
		light->m_transform = light->get_entity().get_component<Transform>();
	}

	void DirectionalLightSystem::on_pre_render(float delta_time)
	{
		for (Handle<DirectionalLight> light : *this)
		{
			light->m_light_data.direction = glm::vec4(light->m_transform->get_forward(), 1.0f);
#if DK_EDITOR
			dk::editor::renderer.draw(light->m_light_data);
#else
			dk::engine::renderer.draw(light->m_light_data);
#endif
		}
	}

	void DirectionalLightSystem::serialize(ComponentArchive& archive)
	{
		Handle<DirectionalLight> light = get_component();
		archive.set_name("Directional Light");
		archive.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		archive.set_field("Intensity", light->m_light_data.color.w);
	}

	void DirectionalLightSystem::inspect(ReflectionContext& context)
	{
		Handle<DirectionalLight> light = get_component();
		context.set_name("Directional Light");
		context.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		context.set_field("Intensity", light->m_light_data.color.w);
	}

	void PointLightSystem::on_begin()
	{
		Handle<PointLight> light = get_component();
		light->m_transform = light->get_entity().get_component<Transform>();
	}

	void PointLightSystem::on_pre_render(float delta_time)
	{
		for (Handle<PointLight> light : *this)
		{
			light->m_light_data.position = glm::vec4(light->m_transform->get_position(), light->m_light_data.position.w);

#if DK_EDITOR
			dk::editor::renderer.draw(light->m_light_data);
#else
			dk::engine::renderer.draw(light->m_light_data);
#endif
		}
	}

	void PointLightSystem::serialize(ComponentArchive& archive)
	{
		Handle<PointLight> light = get_component();
		archive.set_name("Point Light");
		archive.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		archive.set_field("Intensity", light->m_light_data.color.w);
		archive.set_field("Range", light->m_light_data.position.w);
	}

	void PointLightSystem::inspect(ReflectionContext& context)
	{
		Handle<PointLight> light = get_component();
		context.set_name("Point Light");
		context.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		context.set_field("Intensity", light->m_light_data.color.w);
		context.set_field("Range", light->m_light_data.position.w);
	}
}