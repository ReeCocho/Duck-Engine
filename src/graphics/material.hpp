#pragma once

/**
 * @file material.hpp
 * @brief Shader instance.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include <map>
#include "material_shader.hpp"
#include "texture.hpp"

namespace dk
{
	/**
	 * @brief Shader instance.
	 */
	class Material
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Material();

		/**
		 * @brief Constructor.
		 * @param Graphics context.
		 * @param Shader.
		 */
		Material(Graphics* graphics, Handle<MaterialShader> shader);

		/**
		 * @brief Destructor.
		 */
		~Material();

		/**
		 * @brief Free material resources.
		 */
		void free();

		/**
		 * @brief Set texture.
		 * @param Texture index.
		 * @param Texture handle.
		 */
		void set_texture(size_t index, Handle<Texture> texture);

		/**
		 * @brief Set cube map.
		 * @param Cube map index.
		 * @param Cube map handle.
		 */
		void set_cube_map(size_t index, Handle<CubeMap> cube_map);

		/**
		 * @brief Get shader.
		 * @return Shader.
		 */
		Handle<MaterialShader> get_shader()
		{
			return m_shader;
		}

		/**
		 * @brief Get texture.
		 * @param Index.
		 * @return Texture.
		 */
		Handle<Texture> get_texture(size_t index)
		{
			try
			{
				Handle<Texture> handle = m_textures.at(index);
				return handle;
			}
			catch (std::out_of_range e)
			{ return Handle<Texture>(); }
		}

		/**
		 * @brief Get cube map.
		 * @param Index.
		 * @return Cube map.
		 */
		Handle<CubeMap> get_cube_map(size_t index)
		{
			try
			{
				Handle<CubeMap> handle = m_cube_maps.at(index);
				return handle;
			}
			catch (std::out_of_range e)
			{ return Handle<CubeMap>(); }
		}

		/**
		 * @brief Get vertex uniform buffer.
		 * @return Vertex uniform buffer.
		 */
		const VkMemBuffer& get_vertex_uniform_buffer() const
		{
			return m_vertex_uniform_buffer;
		}

		/**
		 * @brief Get fragment uniform buffer.
		 * @return Fragment uniform buffer.
		 */
		const VkMemBuffer& get_fragment_uniform_buffer() const
		{
			return m_fragment_uniform_buffer;
		}

		/**
		 * @brief Get texture descriptor set.
		 * @return Texture descriptor set.
		 */
		const vk::DescriptorSet& get_texture_descriptor_set() const
		{
			return m_vk_texture_descriptor_set;
		}

		/**
		 * @brief Set vertex data.
		 * @tparam Type of data sent.
		 * @param Vertex data.
		 */
		template<typename T>
		void set_vertex_data(const T& vert_data)
		{
			dk_assert(sizeof(T) <= m_shader->get_vertex_buffer_size());
			memcpy(m_vertex_map, &vert_data, sizeof(T));
		}

		/**
		 * @brief Set fragment data.
		 * @tparam Type of data sent.
		 * @param Fragment data.
		 */
		template<typename T>
		void set_fragment_data(const T& frag_data)
		{
			dk_assert(sizeof(T) <= m_shader->get_fragment_buffer_size());
			memcpy(m_fragment_map, &frag_data, sizeof(T));
		}

	private:

		/**
		 * Update texture descriptor set.
		 */
		void update_texture_descriptor_set();



		/** Graphics context. */
		Graphics* m_graphics;

		/** Shader. */
		Handle<MaterialShader> m_shader;

		/** Descriptor pool. */
		vk::DescriptorPool m_vk_descriptor_pool = {};

		/** Texture descriptor set. */
		vk::DescriptorSet m_vk_texture_descriptor_set = {};

		/** Vertex uniform buffer. */
		VkMemBuffer m_vertex_uniform_buffer;

		/** Fragment uniform buffer. */
		VkMemBuffer m_fragment_uniform_buffer;

		/** Fragment mapping. */
		void* m_fragment_map;

		/** Vertex mapping. */
		void* m_vertex_map;

		/** Textures. */
		std::map<size_t, Handle<Texture>> m_textures = {};

		/** Cube maps. */
		std::map<size_t, Handle<CubeMap>> m_cube_maps = {};
	};

	/** Handle to a material. */
	using HMaterial = Handle<Material>;
}