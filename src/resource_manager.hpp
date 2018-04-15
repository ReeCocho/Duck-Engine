#pragma once

/**
 * @file resource_manager.hpp
 * @brief Resource manager header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\resource_allocator.hpp>
#include <graphics\renderer.hpp>
#include <graphics\material.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\texture.hpp>

namespace dk
{
	/**
	 * @brief Allocate and manage resources.
	 */
	class ResourceManager
	{
	public:
		
		/**
		 * @brief Default constructor.
		 */
		ResourceManager() = default;

		/**
		 * @brief Constructor.
		 * @param Renderer.
		 */
		ResourceManager(Renderer* renderer);

		/**
		 * @brief Shutdown the resource manager.
		 */
		void shutdown();

		/**
		 * @brief Get mesh allocator.
		 * @return Mesh allocator.
		 */
		ResourceAllocator<Mesh>& get_mesh_allocator()
		{
			return *m_mesh_allocator.get();
		}

		/**
		 * @brief Get shader allocator.
		 * @return Shader allocator.
		 */
		ResourceAllocator<Shader>& get_shader_allocator()
		{
			return *m_shader_allocator.get();
		}

		/**
		 * @brief Get material allocator.
		 * @return Material allocator.
		 */
		ResourceAllocator<Material>& get_material_allocator()
		{
			return *m_material_allocator.get();
		}

		/**
		 * @brief Get texture allocator.
		 * @return Texture allocator.
		 */
		ResourceAllocator<Texture>& get_texture_allocator()
		{
			return *m_texture_allocator.get();
		}

		/**
		 * @brief Create a mesh.
		 * @param Indices.
		 * @param Vertices.
		 * @return Mesh handle.
		 */
		Handle<Mesh> create_mesh(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);

		/**
		 * @brief Create a shader.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 * @return Shader handle.
		 */
		Handle<Shader> create_shader(const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code);

		/**
		 * @brief Create a material.
		 * @param Shader handle.
		 * @return Material handle.
		 */
		Handle<Material> create_material(Handle<Shader> shader);

		/**
		 * @brief Create a texture.
		 * @param Path to texture.
		 * @param Filtering mode.
		 * @return Texture handle.
		 */
		Handle<Texture> create_texture(const std::string& path, vk::Filter filtering);

		/**
		 * @brief Destroy a mesh.
		 * @param Mesh handle.
		 */
		void destroy(Handle<Mesh> mesh);

		/**
		 * @brief Destroy a shader.
		 * @param Shader handle.
		 */
		void destroy(Handle<Shader> shader);

		/**
		 * @brief Destroy a material.
		 * @param Material handle.
		 */
		void destroy(Handle<Material> material);

		/**
		 * @brief Destroy a texture.
		 * @param Texture handle.
		 */
		void destroy(Handle<Texture> texture);

	private:
		
		/** Rendering engine. */
		Renderer* m_renderer;

		/** Mesh allocator. */
		std::unique_ptr<ResourceAllocator<Mesh>> m_mesh_allocator;

		/** Shader allocator. */
		std::unique_ptr<ResourceAllocator<Shader>> m_shader_allocator;

		/** Material allocator. */
		std::unique_ptr<ResourceAllocator<Material>> m_material_allocator;

		/** Texture allocator. */
		std::unique_ptr<ResourceAllocator<Texture>> m_texture_allocator;
	};
}