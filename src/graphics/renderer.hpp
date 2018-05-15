#pragma once

/**
 * @file renderer.hpp
 * @brief Duck renderer base.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include "graphics.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "sky_box.hpp"

namespace dk
{
	/**
	 * @brief Camera data structure.
	 */
	struct CameraData
	{
		/** View-projection matrix. */
		glm::mat4 vp_mat = {};

		/** Position in space. */
		glm::vec3 position = {};

		/** View frustum. */
		Frustum frustum = {};

		/** Command buffers to use for rendering the skybox. */
		std::vector<VkManagedCommandBuffer> command_buffers = {};

		/** Sky box. */
		Handle<SkyBox> sky_box = Handle<SkyBox>();
	};

	/**
	 * @brief An object that can be rendered onto the screen.
	 */
	struct RenderableObject
	{
		/** Command buffer to record to. */
		std::vector<VkManagedCommandBuffer> command_buffers = {};

		/** Shader. */
		Handle<Shader> shader = {};

		/** Mesh. */
		Handle<Mesh> mesh = {};

		/** Descriptor sets. */
		std::vector<vk::DescriptorSet> descriptor_sets = {};

		/** Model matrix. */
		glm::mat4 model = {};
	};

	/**
	 * @brief Renderer base class.
	 */
	class Renderer
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Renderer() = default;

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 */
		Renderer(Graphics* graphics);

		/**
		 * @brief Destructor.
		 */
		virtual ~Renderer() = default;

		/**
		 * @brief Shutdown the renderer.
		 */
		virtual void shutdown() = 0;

		/**
		 * @brief Get graphics context.
		 * @return Graphics context.
		 */
		Graphics& get_graphics() const
		{
			return *m_graphics;
		}

		/**
		 * @brief Get main camera.
		 * @return Main camera
		 */
		const CameraData& get_main_camera() const
		{
			return m_main_camera;
		}

		/**
		 * @brief Draw a renderable object.
		 * @param Renderable object.
		 */
		virtual void draw(const RenderableObject& obj) = 0;

		/**
		* @brief Set main camera.
		* @param Camera data.
		*/
		virtual void set_main_camera(const CameraData& data) = 0;

		/**
		 * @brief Render everything.
		 */
		virtual void render() = 0;

	private:

		/**
		 * @brief Copy constructor.
		 * @param Other rendering engine.
		 */
		Renderer(const Renderer& other) = default;

		/**
		 * @brief Assignment operator.
		 * @param Other rendering engine.
		 * @return This.
		 */
		Renderer& operator=(const Renderer& other) { return *this; };



		/** Graphics context. */
		Graphics* m_graphics;

	protected:

		/** Main camera data. */
		CameraData m_main_camera;

		/** List of renderable objects. */
		std::vector<RenderableObject> m_renderable_objects;
	};
}