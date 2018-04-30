#pragma once

/**
 * @file lighting.hpp
 * @brief Duck lighting manager.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include "graphics.hpp"

namespace dk
{
	/**
	 * @brief Directional light data.
	 */
	struct DirectionalLightData
	{
		/** Direction. */
		glm::vec4 direction = {};

		/** Color. (W is intensity) */
		glm::vec4 color = {};
	};

	/**
	 * @brief Point light data.
	 */
	struct PointLightData
	{
		/** Position. (W is range) */
		glm::vec4 position = {};

		/** Color. (W is intensity) */
		glm::vec4 color = {};
	};



	/**
	 * @brief Manages lights and their buffers.
	 */
	class LightingManager
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Starting number of point lights.
		 * @param Starting number of directional lights.
		 */
		LightingManager(Graphics* graphics, size_t point_light_count, size_t dir_light_count);

		/**
		 * @brief Destructor.
		 */
		~LightingManager();

		/**
		 * @brief Upload lighting data.
		 */
		void upload();

		/**
		 * @brief Flush light queues.
		 */
		void flush_queues();

		/**
		 * @brief Get lighting data UBO.
		 * @return Lighting data UBO.
		 */
		VkMemBuffer& get_lighting_data_ubo()
		{
			return m_lighting_ubo;
		}

		/**
		 * @brief Get point light SSBO.
		 * @return Point light SSBO.
		 */
		VkMemBuffer& get_point_light_ssbo()
		{
			return m_point_light_ssbo;
		}

		/**
		 * @brief Get directional light SSBO.
		 * @return Directional light SSBO.
		 */
		VkMemBuffer& get_directional_light_ssbo()
		{
			return m_directional_light_ssbo;
		}

		/**
		 * @brief Get lighting data size.
		 * @return Lighting data size.
		 */
		size_t get_lighting_data_size() const
		{
			return sizeof(m_lighting_data);
		}

		/**
		 * @brief Get point light data size.
		 * @return Point light data size.
		 */
		size_t get_point_light_data_size() const
		{
			return sizeof(uint32_t) + (sizeof(PointLightData) * m_point_lights.size());
		}

		/**
		 * @brief Get directional light data size.
		 * @return Directional light data size.
		 */
		size_t get_directional_light_data_size() const
		{
			return sizeof(uint32_t) + (sizeof(DirectionalLightData) * m_directional_lights.size());
		}

		/**
		 * @brief Set ambient light color.
		 * @param Color.
		 */
		void set_ambient_color(glm::vec3 color)
		{
			m_lighting_data.ambient = glm::vec4(color, m_lighting_data.ambient.w);
		}

		/**
		 * @brief Set ambient light intensity.
		 * @param Intensity.
		 */
		void set_ambient_intensity(float i)
		{
			m_lighting_data.ambient.w = i;
		}

		/**
		 * @brief Set camera position.
		 * @param Camera position.
		 */
		void set_camera_position(glm::vec3 cam_pos)
		{
			m_lighting_data.camera_position = glm::vec4(cam_pos, 1.0f);
		}

		/**
		 * @brief Draw point light.
		 * @param Point light data.
		 */
		void draw(PointLightData data);

		/**
		 * @brief Draw directional light.
		 * @param Directional light data.
		 */
		void draw(DirectionalLightData data);

	private:

		/**
		 * @brief Create point light SSBO.
		 */
		void create_point_light_ssbo();

		/**
		 * @brief Create directional light SSBO.
		 */
		void create_directional_ssbo();

		/**
		 * @brief Destroy point light SSBO.
		 */
		void destroy_point_light_ssbo();

		/**
		 * @brief Destroy directional light SSBO.
		 */
		void destroy_directional_light_ssbo();



		/** Graphics context. */
		Graphics* m_graphics;

		/** Point lights. */
		std::vector<PointLightData> m_point_lights = {};
		
		/** Point lights allocated. */
		size_t m_point_light_count = 0;

		/** Directional lights. */
		std::vector<DirectionalLightData> m_directional_lights = {};

		/** Directional lights allocated. */
		size_t m_directional_light_count = 0;

		/**
		 * @brief Lighting data
		 */
		struct
		{
			/** Ambient color (W value is intensity.) */
			glm::vec4 ambient = { 1.0f, 1.0f, 1.0f, 0.2f };

			/** Camera position. */
			glm::vec4 camera_position = {};

		} m_lighting_data;

		/** Point light SSBO. */
		VkMemBuffer m_point_light_ssbo = {};

		/** Directional light SSBO. */
		VkMemBuffer m_directional_light_ssbo = {};

		/** Lighting uniform buffer. */
		VkMemBuffer m_lighting_ubo = {};

		/** Lighting data UBO mapping pointer. */
		void* m_lighting_map = nullptr;

		/** Point light data SSBO mapping pointer. */
		void* m_point_light_map = nullptr;

		/** Directional data SSBO mapping pointer. */
		void* m_directional_light_map = nullptr;
	};
}