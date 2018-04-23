#pragma once

/**
 * @file camera.hpp
 * @brief Component that describes a view in space.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include "engine.hpp"
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

		/**
		 * @brief Default constructor.
		 */
		Camera() : Component<Camera>(Handle<Camera>(0, nullptr), Entity()) {}

		/**
		 * @brief Constructor.
		 * @param Components handle.
		 * @param Entity the component belongs to.
		 */
		Camera(Handle<Camera> handle, Entity entity) : Component<Camera>(handle, entity) {}

		/**
		 * @brief Get projection-view matrix.
		 * @return Projection-view matrix.
		 */
		glm::mat4 get_pv_matrix() const
		{
			return m_projection * m_view;
		}

		/**
		 * @brief Check if a point is inside the cameras frustum.
		 * @param Point.
		 * @return If the point is inside.
		 */
		bool check_inside_frustum(glm::vec3 p);

		/**
		 * @brief Check if a sphere intersects or is inside the cameras frustum.
		 * @param Center.
		 * @param Radius.
		 * @return If the sphere is inside/intersects.
		 */
		bool check_inside_frustum(glm::vec3 c, float r);

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
		 * @brief Set field of view.
		 * @param New field of view.
		 * @return New field of view.
		 */
		float set_fov(float fov)
		{
			m_field_of_view = fov;
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
			return m_far_clipping_plane;
		}

	private:

		/** Transform. */
		Handle<Transform> m_transform = {};

		/** Projection furstum. */
		glm::mat4 m_projection = {};

		/** View matrix. */
		glm::mat4 m_view = {};

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
		CameraSystem(Scene* scene) : System<Camera>(scene, 4) {}

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