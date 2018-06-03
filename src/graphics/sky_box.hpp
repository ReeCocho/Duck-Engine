#pragma once

/**
 * @file sky_box.hpp
 * @brief Sky box class.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include "graphics.hpp"
#include "material.hpp"
#include "mesh.hpp"

namespace dk
{
	/**
	 * Skybox used in renderers.
	 */
	class SkyBox
	{
	public:

		/**
		 * Default constructor.
		 */
		SkyBox() = default;

		/**
		 * Constructor.
		 * @param Graphics context.
		 */
		SkyBox(Graphics* graphics);

		/**
		 * Default destructor.
		 */
		~SkyBox() = default;

		/**
		 * Free the skybox.
		 */
		void free();

		/**
		 * Get mesh.
		 * @return Mesh.
		 */
		Handle<Mesh> get_mesh() const
		{
			return m_mesh;
		}

		/**
		 * Get material.
		 * @return Material.
		 */
		Handle<Material> get_material() const
		{
			return m_material;
		}

		/**
		 * Get descriptor set.
		 * @return Descriptor set.
		 */
		const vk::DescriptorSet& get_descriptor_set() const
		{
			return m_vk_descriptor_set;
		}

		/**
		 * Set mesh.
		 * @param New mesh.
		 * @return New mesh.
		 */
		Handle<Mesh> set_mesh(Handle<Mesh> mesh);

		/**
		 * Set material.
		 * @param New material.
		 * @return New material.
		 */
		Handle<Material> set_material(Handle<Material> material);

		/**
		 * Set vertex data.
		 * @param Vertex data.
		 */
		void set_vertex_data(const VertexShaderData& data);

	private:

		/**
		 * @brief Generate resources.
		 */
		void generate_resources();


		
		/** Graphics context. */
		Graphics* m_graphics;

		/** Mesh. */
		Handle<Mesh> m_mesh = Handle<Mesh>();

		/** Material. */
		Handle<Material> m_material = Handle<Material>();

		/** Descriptor pool. */
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

	/** Handle to a sky box. */
	using HSkyBox = Handle<SkyBox>;
}