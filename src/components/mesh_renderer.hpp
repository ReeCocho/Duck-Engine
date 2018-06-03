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
#include "transform.hpp"

namespace dk
{
	/**
	 * @brief Component that renders meshes.
	 */
	class MeshRenderer : public Component<MeshRenderer>
	{
		friend class MeshRendererSystem;

	public:

		DK_COMPONENT_BODY(MeshRenderer)

		/**
		 * @brief Set the material.
		 * @param New material.
		 * @return New material.
		 */
		HMaterial set_material(HMaterial material)
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
		HMesh set_mesh(HMesh mesh)
		{
			m_mesh = mesh;
			generate_resources();
			return m_mesh;
		}

		/**
		 * @brief Get the material.
		 * @return The material.
		 */
		HMaterial get_material()
		{
			return m_material;
		}

		/**
		 * @brief Get the mesh.
		 * @return The mesh.
		 */
		HMesh get_mesh()
		{
			return m_mesh;
		}

	private:

		/**
		 * @brief Generate resources.
		 */
		void generate_resources();

		/**
		 * @brief Free resources.
		 */
		void free_resources();

		

		/** Transform. */
		Handle<Transform> m_transform = {};

		/** Material the mesh renders using. */
		HMaterial m_material = {};

		/** Mesh used when rendering. */
		HMesh m_mesh = {};

		/** Command buffer used for rendering. */
		VkManagedCommandBuffer m_command_buffer = {};

		/** Depth prepass command buffer. */
		VkManagedCommandBuffer m_depth_prepass_command_buffer = {};

		/** Meshes descriptor pool. */
		vk::DescriptorPool m_vk_descriptor_pool = {};

		/** Descriptor set. */
		vk::DescriptorSet m_vk_descriptor_set = {};

		/** Per instance vertex uniform buffer. */
		VkMemBuffer m_vertex_uniform_buffer = {};

		/** Per instance fragment uniform buffer. */
		VkMemBuffer m_fragment_uniform_buffer = {};

		/** Vertex buffer mapping. */
		void* m_vertex_map = nullptr;

		/** Fragment buffer mapping. */
		void* m_fragment_map = nullptr;
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
		MeshRendererSystem(Scene* scene) : System<MeshRenderer>(scene, true, 32) {}

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

		/**
		 * Serialize a component.
		 * @param Archiver.
		 */
		void serialize(ReflectionContext& archive) override;
	};
}