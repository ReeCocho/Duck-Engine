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
			return m_proj_view;
		}

	private:

		/** Transform. */
		Handle<Transform> m_transform = {};

		/** Virtual camera from renderer. */
		Handle<VirtualCamera> m_virtual_camera = {};

		/** Projection-view matrix. */
		glm::mat4 m_proj_view = {};

		/** Field of view. */
		float m_field_of_view = 80.0f;

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