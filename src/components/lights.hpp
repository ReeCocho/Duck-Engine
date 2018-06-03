#pragma once

/**
 * @file lights.hpp
 * @brief Components that emit light.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <engine\common.hpp>
#include <ecs\scene.hpp>
#include "transform.hpp"

namespace dk
{
	/**
	 * @brief Directional light.
	 */
	class DirectionalLight : public Component<DirectionalLight>
	{
		friend class DirectionalLightSystem;

	public:

		DK_COMPONENT_BODY(DirectionalLight)

		/**
		 * @brief Set color.
		 * @param New color.
		 * @return New color.
		 */
		glm::vec3 set_color(glm::vec3 color)
		{
			m_light_data.color = glm::vec4(color, m_light_data.color.w);
			return color;
		}

		/**
		 * @brief Set intensity.
		 * @param New intensity.
		 * @return New intensity.
		 */
		float set_intensity(float intensity)
		{
			m_light_data.color.w = intensity;
			return m_light_data.color.w;
		}

	private:

		/** Transform component. */
		Handle<Transform> m_transform;

		/** Light data. */
		DirectionalLightData m_light_data = {};
	};

	/**
	 * @brief Point light.
	 */
	class PointLight : public Component<PointLight>
	{
		friend class PointLightSystem;

	public:

		DK_COMPONENT_BODY(PointLight)

		/**
		 * @brief Set color.
		 * @param New color.
		 * @return New color.
		 */
		glm::vec3 set_color(glm::vec3 color)
		{
			m_light_data.color = glm::vec4(color, m_light_data.color.w);
			return color;
		}

		/**
		 * @brief Set intensity.
		 * @param New intensity.
		 * @return New intensity.
		 */
		float set_intensity(float intensity)
		{
			m_light_data.color.w = intensity;
			return m_light_data.color.w;
		}

		/**
		 * @brief Set range.
		 * @param New range.
		 * @return New range.
		 */
		float set_range(float range)
		{
			m_light_data.position.w = range;
			return m_light_data.position.w;
		}

	private:

		/** Transform component. */
		Handle<Transform> m_transform;

		/** Light data. */
		PointLightData m_light_data = {};
	};



	/**
	 * @brief Implementation of directional lights.
	 */
	class DirectionalLightSystem : public System<DirectionalLight>
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 */
		DirectionalLightSystem(Scene* scene) : System<DirectionalLight>(scene, true, 16) {}

		/**
		 * @brief Destructor.
		 */
		~DirectionalLightSystem() = default;

		/**
		 * @brief Called when a component is added to the system.
		 */
		void on_begin() override;

		/**
		 * @brief Called after on_late_tick() and before rendering.
		 * @param Time in seconds since the last frame.
		 */
		void on_pre_render(float delta_time) override;

		/**
		 * Serialize a component.
		 * @param Archiver.
		 */
		void serialize(ReflectionContext& archive) override;
	};

	/**
	 * @brief Implementation of directional lights.
	 */
	class PointLightSystem : public System<PointLight>
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 */
		PointLightSystem(Scene* scene) : System<PointLight>(scene, true, 16) {}

		/**
		 * @brief Destructor.
		 */
		~PointLightSystem() = default;

		/**
		 * @brief Called when a component is added to the system.
		 */
		void on_begin() override;

		/**
		 * @brief Called after on_late_tick() and before rendering.
		 * @param Time in seconds since the last frame.
		 */
		void on_pre_render(float delta_time) override;

		/**
		 * Serialize a component.
		 * @param Archiver.
		 */
		void serialize(ReflectionContext& archive) override;
	};
}