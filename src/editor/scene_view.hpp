#pragma once

/**
 * @file scene_view.hpp
 * @brief Scene view header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <graphics\graphics.hpp>
#include <graphics\forward_renderer.hpp>
#include <components\transform.hpp>
#include <ecs\scene.hpp>
#include <input.hpp>
#include "editor_renderer.hpp"
#include "imgui\imgui.h"

namespace dk
{
	class SceneView
	{
	public:

		/**
		 * Constructor.
		 * @param Forward renderer to display.
		 * @param Editor renderer to display on.
		 * @param Input manager.
		 */
		SceneView(OffScreenForwardRenderer* renderer, EditorRenderer* editor_renderer, Input* input);

		/**
		 * Destructor.
		 */
		~SceneView();

		/**
		 * Display the scene view.
		 * @parma Delta time.
		 */
		void draw(float delta_time);

	private:

		/**
		 * Generate a view-projection matrix for a camera.
		 * @param Window aspect ratio.
		 * @param Forward vector.
		 * @param Up vector.
		 * @return View-projection matrix.
		 */
		glm::mat4 generate_vp_matrix(float aspect_ratio, glm::vec3 forward, glm::vec3 up);



		/** Renderer. */
		OffScreenForwardRenderer* m_renderer;

		/** Editor renderer. */
		EditorRenderer* m_editor_renderer;

		/** Input manager. */
		Input* m_input;

		/** Descriptor pool. */
		vk::DescriptorPool m_vk_descriptor_pool;

		/** Descriptor set. */
		vk::DescriptorSet m_vk_descriptor_set;

		/**
		 * Scene view camera data.
		 */
		struct
		{
			/** Position. */
			glm::vec3 position = { 0, 0, 0 };

			/** Rotation (Euler angles) */
			glm::vec3 rotation = { 0, 0, 0 };

			/** Field of view. */
			float field_of_view = 80.0f;

			/** Near clipping plane. */
			float near_clipping = 0.03f;

			/** Far clipping plane. */
			float far_clipping = 1000.0f;

			/** Command buffers. */
			std::vector<VkManagedCommandBuffer> command_buffers = {};

		} m_camera;
	};
}