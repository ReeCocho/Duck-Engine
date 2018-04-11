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
		 * @brief Set the material.
		 * @param New material.
		 * @return New material.
		 */
		Material* set_material(Material* material)
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
		Mesh* set_mesh(Mesh* mesh)
		{
			m_mesh = mesh;
			generate_resources();
			return m_mesh;
		}

		/**
		 * @brief Get the material.
		 * @return The material.
		 */
		Material* get_material()
		{
			return m_material;
		}

		/**
		 * @brief Get the mesh.
		 * @return The mesh.
		 */
		Mesh* get_mesh()
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
		Material* m_material = nullptr;

		/** Mesh used when rendering. */
		Mesh* m_mesh = nullptr;

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