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
		Handle<DirectionalLight> light = get_active_component();
		light->m_transform = light->get_entity().get_component<Transform>();
	}

	void DirectionalLightSystem::on_pre_render(float delta_time)
	{
		for (Handle<DirectionalLight> light : *this)
		{
			light->m_light_data.direction = glm::vec4(light->m_transform->get_forward(), 1.0f);
			dk::engine::renderer.draw(light->m_light_data);
		}
	}

	void DirectionalLightSystem::serialize(ReflectionContext& r)
	{
		Handle<DirectionalLight> light = get_active_component();
		r.set_name("Directional Light");
		r.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		r.set_field("Intensity", light->m_light_data.color.w);
	}

	void DirectionalLightSystem::inspect(ReflectionContext& r)
	{
		Handle<DirectionalLight> light = get_active_component();
		r.set_name("Directional Light");
		r.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		r.set_field("Intensity", light->m_light_data.color.w);
	}

	void PointLightSystem::on_begin()
	{
		Handle<PointLight> light = get_active_component();
		light->m_transform = light->get_entity().get_component<Transform>();
	}

	void PointLightSystem::on_pre_render(float delta_time)
	{
		for (Handle<PointLight> light : *this)
		{
			light->m_light_data.position = glm::vec4(light->m_transform->get_position(), light->m_light_data.position.w);
			dk::engine::renderer.draw(light->m_light_data);
		}
	}

	void PointLightSystem::serialize(ReflectionContext& r)
	{
		Handle<PointLight> light = get_active_component();
		r.set_name("Point Light");
		r.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		r.set_field("Intensity", light->m_light_data.color.w);
		r.set_field("Range", light->m_light_data.position.w);
	}

	void PointLightSystem::inspect(ReflectionContext& r)
	{
		Handle<PointLight> light = get_active_component();
		r.set_name("Point Light");
		r.set_field("Color", *(glm::vec3*)&light->m_light_data.color);
		r.set_field("Intensity", light->m_light_data.color.w);
		r.set_field("Range", light->m_light_data.position.w);
	}
}