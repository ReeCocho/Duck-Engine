#pragma once

/**
 * @file camera.hpp
 * @brief Component that describes a view in space.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <common.hpp>
#include <ecs\scene.hpp>

namespace dk
{
	/**
	 * @brief Component that describes a view in space.
	 */
	class Camera : public Component<Camera>
	{
		friend class CameraSystem;

	public:

		DK_COMPONENT_BODY(Camera)

		/**
		 * @brief Get projection-view matrix.
		 * @return Projection-view matrix.
		 */
		glm::mat4 get_pv_matrix() const
		{
			return m_projection * m_view;
		}

		/**
		 * @brief Get field of view.
		 * @return Field of view.
		 */
		float get_fov() const
		{
			return m_field_of_view;
		}

		/**
		 * @brief Get near clipping plane.
		 * @return Near clipping plane.
		 */
		float get_near_clipping_plane() const
		{
			return m_near_clipping_plane;
		}

		/**
		 * @brief Get far clipping plane.
		 * @return Far clipping plane.
		 */
		float get_far_clipping_plane() const
		{
			return m_far_clipping_plane;
		}

		/**
		 * @brief Get view frustum.
		 * @return View frustum.
		 */
		Frustum get_view_frustum() const
		{
			return m_view_frustum;
		}

		/**
		 * Get sky box.
		 * @return Sky box.
		 */
		Handle<SkyBox> get_sky_box() const
		{
			return m_sky_box;
		}

		/**
		 * @brief Set field of view.
		 * @param New field of view.
		 * @return New field of view.
		 */
		float set_fov(float fov)
		{
			m_field_of_view = fov;
			calculate_vp_matrices();
			calculate_frustum();
			return m_field_of_view;
		}

		/**
		 * @brief Set near clipping plane.
		 * @param New near clipping plane.
		 * @return New near clipping plane.
		 */
		float set_near_clipping_plane(float ncp)
		{
			m_near_clipping_plane = ncp;
			calculate_vp_matrices();
			calculate_frustum();
			return m_near_clipping_plane;
		}

		/**
		 * @brief Set far clipping plane.
		 * @param New far clipping plane.
		 * @return New far clipping plane.
		 */
		float set_far_clipping_plane(float fcp)
		{
			m_far_clipping_plane = fcp;
			calculate_vp_matrices();
			calculate_frustum();
			return m_far_clipping_plane;
		}

		/**
		 * Set sky box.
		 * @param New sky box.
		 * @return New sky box.
		 */
		Handle<SkyBox> set_sky_box(Handle<SkyBox> sky_box)
		{
			m_sky_box = sky_box;
			return m_sky_box;
		}

	private:

		/**
		 * @brief Calculate the view and perspective matrices.
		 */
		void calculate_vp_matrices();

		/**
		 * @brief Calculate the view frustum planes.
		 */
		void calculate_frustum();



		/** Transform. */
		Handle<Transform> m_transform = {};

		/** Projection furstum. */
		glm::mat4 m_projection = {};

		/** View matrix. */
		glm::mat4 m_view = {};

		/** View frustum. */
		Frustum m_view_frustum = {};

		/** Command buffers for rendering the skybox. */
		std::vector<VkManagedCommandBuffer> m_command_buffers = {};

		/** Skybox. */
		Handle<SkyBox> m_sky_box = {};

		/** Field of view. */
		float m_field_of_view = 100.0f;

		/** Near clipping plane. */
		float m_near_clipping_plane = 0.03f;

		/** Far clipping plane. */
		float m_far_clipping_plane = 100.0f;
	};

	/**
	 * @brief Implementation of the camera.
	 */
	class CameraSystem : public System<Camera>
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 */
		CameraSystem(Scene* scene) : System<Camera>(scene, false, 4) {}

		/**
		 * @brief Destructor.
		 */
		~CameraSystem() = default;

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
		 * @brief Called when a component is removed from the system.
		 */
		void on_end() override;

		/**
		 * Serialize a component.
		 * @param Archiver.
		 */
		void serialize(ReflectionContext& archive) override;

		/**
		 * @brief Set the main camera.
		 * @param New main camera.
		 */
		static void set_main_camera(Handle<Camera> camera);
		
		/**
		 * @brief Get the main camera.
		 * @return The main camera.
		 */
		static Handle<Camera> get_main_camera();

	private:

		/** Main camera. */
		static Handle<Camera> main_camera;
	};
}