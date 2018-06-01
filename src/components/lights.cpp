/**
 * @file lights.cpp
 * @brief Lights source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "transform.hpp"
#include <config.hpp>
#include "lights.hpp"
#include <common.hpp>

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

	void DirectionalLightSystem::serialize(ReflectionContext& archive)
	{
		ComponentArchive& a = static_cast<ComponentArchive&>(archive);
		Handle<DirectionalLight> light = get_component();
		a.set_name("Directional Light");
		a.field<glm::vec3>("Color", (glm::vec3*)&light->m_light_data.color);
		a.field<float>("Intensity", &light->m_light_data.color.w);
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

	void PointLightSystem::serialize(ReflectionContext& archive)
	{
		ComponentArchive& a = static_cast<ComponentArchive&>(archive);
		Handle<PointLight> light = get_component();
		a.set_name("Point Light");
		a.field<glm::vec3>("Color", (glm::vec3*)&light->m_light_data.color);
		a.field<float>("Intensity", &light->m_light_data.color.w);
		a.field<float>("Range", &light->m_light_data.position.w);
	}
}