#pragma once

/**
 * @file mesh_renderer.hpp
 * @brief Component that renders meshes.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <ecs\scene.hpp>
#include <graphics\material.hpp>
#include <graphics\mesh.hpp>

namespace dk
{
	struct VertexShaderData
	{
		glm::mat4 mvp;
	};

	struct FragmentShaderData
	{
		int unused;
	};

	/**
	 * @brief Component that renders meshes.
	 */
	class MeshRenderer : public Component<MeshRenderer>
	{
		friend class MeshRendererSystem;

	public:

		/**
		 * @brief Default constructor.
		 */
		MeshRenderer();

		/**
		 * @brief Constructor.
		 * @param Scene the component belongs to.
		 * @param Entity the component belongs to.
		 */
		MeshRenderer(Scene* scene, Entity entity) : Component<MeshRenderer>(scene, entity) {}

		/**
		 * @brief Set the material.
		 * @param New material.
		 * @return New material.
		 */
		Handle<Material> set_material(Handle<Material> material)
		{
			m_material = material;
			generate_resources();
			return m_material;
		}

		/**
		 * @brief Set the mesh.
		 * @param New mesh.
		 * @return New mesh.
		 */
		Handle<Mesh> set_mesh(Handle<Mesh> mesh)
		{
			m_mesh = mesh;
			generate_resources();
			return m_mesh;
		}

		/**
		 * @brief Get the material.
		 * @return The material.
		 */
		Handle<Material> get_material()
		{
			return m_material;
		}

		/**
		 * @brief Get the mesh.
		 * @return The mesh.
		 */
		Handle<Mesh> get_mesh()
		{
			return m_mesh;
		}

		/** Model matrix. */
		glm::mat4 mvp;

	private:

		/**
		 * @brief Generate resources.
		 */
		void generate_resources();

		

		/** Material the mesh renders using. */
		Handle<Material> m_material = {};

		/** Mesh used when rendering. */
		Handle<Mesh> m_mesh = {};

		/** Command buffer used for rendering. */
		VkManagedCommandBuffer m_command_buffer;

		/** Meshes descriptor pool. */
		vk::DescriptorPool m_vk_descriptor_pool;

		/** Descriptor set. */
		vk::DescriptorSet m_vk_descriptor_set;

		/** Per instance vertex uniform buffer. */
		VkMemBuffer m_vertex_uniform_buffer;

		/** Per instance fragment uniform buffer. */
		VkMemBuffer m_fragment_uniform_buffer;
	};

	/**
	 * @brief Implementation of the mesh renderer.
	 */
	class MeshRendererSystem : public System<MeshRenderer>
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 */
		MeshRendererSystem(Scene* scene) : System<MeshRenderer>(scene, 32) {}

		/**
		 * @brief Destructor.
		 */
		~MeshRendererSystem() = default;

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
	};
}