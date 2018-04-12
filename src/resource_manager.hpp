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

	private:
		
		/** Rendering engine. */
		Renderer* m_renderer;

		/** Mesh allocator. */
		std::unique_ptr<ResourceAllocator<Mesh>> m_mesh_allocator;

		/** Shader allocator. */
		std::unique_ptr<ResourceAllocator<Shader>> m_shader_allocator;

		/** Material allocator. */
		std::unique_ptr<ResourceAllocator<Material>> m_material_allocator;
	};
}