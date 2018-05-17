#pragma once

/**
 * @file resource_manager.hpp
 * @brief Resource manager header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <unordered_map>
#include <utilities\resource_allocator.hpp>
#include <graphics\forward_renderer.hpp>
#include <graphics\material.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\texture.hpp>
#include <graphics\sky_box.hpp>

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
		ResourceManager(ForwardRenderer* renderer);

		/**
		 * @brief Shutdown the resource manager.
		 */
		void shutdown();

		/**
		 * @brief Load resource files.
		 * @param Path to folder containing meshes.
		 * @param Path to folder containing textures.
		 * @param Path to folder containing shaders.
		 * @param Path to folder containing materials.
		 * @param Path to folder containing cube maps.
		 * @param Path to folder containing sky boxes.
		 */
		void load_resources
		(
			const std::string& meshes,
			const std::string& textures,
			const std::string& shaders,
			const std::string& materials,
			const std::string& cube_maps,
			const std::string& sky_boxes
		);

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
		 * Get sky box allocator.
		 * @return Sky box allocator.
		 */
		ResourceAllocator<SkyBox>& get_sky_box_allocator()
		{
			return *m_sky_box_allocator.get();
		}

		/**
		 * Get cube map allocator.
		 * @return Cube map allocator.
		 */
		ResourceAllocator<CubeMap>& get_cube_map_allocator()
		{
			return *m_cube_map_allocator.get();
		}

		/**
		 * @brief Get a mesh.
		 * @param Name.
		 * @param Mesh handle.
		 */
		Handle<Mesh> get_mesh(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_mesh_map.at(name); }
			catch (std::out_of_range e) { return Handle<Mesh>(0, nullptr); }
			return Handle<Mesh>(id, m_mesh_allocator.get());
		}

		/**
		 * @brief Get a shader.
		 * @param Name.
		 * @param Shader handle.
		 */
		Handle<Shader> get_shader(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_shader_map.at(name); }
			catch (std::out_of_range e) { return Handle<Shader>(0, nullptr); }
			return Handle<Shader>(id, m_shader_allocator.get());
		}

		/**
		 * @brief Get a material.
		 * @param Name.
		 * @param Material handle.
		 */
		Handle<Material> get_material(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_material_map.at(name); }
			catch (std::out_of_range e) { return Handle<Material>(0, nullptr); }
			return Handle<Material>(id, m_material_allocator.get());
		}

		/**
		 * @brief Get a texture.
		 * @param Name.
		 * @param Texture handle.
		 */
		Handle<Texture> get_texture(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_texture_map.at(name); }
			catch (std::out_of_range e) { return Handle<Texture>(0, nullptr); }
			return Handle<Texture>(id, m_texture_allocator.get());
		}

		/**
		 * @brief Get a sky box.
		 * @param Name.
		 * @param Sky box handle.
		 */
		Handle<SkyBox> get_sky_box(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_sky_box_map.at(name); }
			catch (std::out_of_range e) { return Handle<SkyBox>(0, nullptr); }
			return Handle<SkyBox>(id, m_sky_box_allocator.get());
		}

		/**
		 * @brief Get a cube map.
		 * @param Name.
		 * @param Cube map handle.
		 */
		Handle<CubeMap> get_cube_map(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_cube_map_map.at(name); }
			catch (std::out_of_range e) { return Handle<CubeMap>(0, nullptr); }
			return Handle<CubeMap>(id, m_cube_map_allocator.get());
		}

		/**
		 * @brief Create a mesh.
		 * @param Name.
		 * @param Indices.
		 * @param Vertices.
		 * @return Mesh handle.
		 */
		Handle<Mesh> create_mesh(const std::string& name, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);

		/**
		 * @brief Create a mesh.
		 * @param Name.
		 * @param Path to the OBJ file.
		 * @return Mesh handle.
		 */
		Handle<Mesh> create_mesh(const std::string& name, const std::string& path);

		/**
		 * @brief Create a shader.
		 * @param Name.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 * @param Should the shader perform depth testing?
		 * @return Shader handle.
		 */
		Handle<Shader> create_shader(const std::string& name, const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code, bool depth = true);

		/**
		 * @brief Create a material.
		 * @param Name.
		 * @param Shader handle.
		 * @return Material handle.
		 */
		Handle<Material> create_material(const std::string& name, Handle<Shader> shader);

		/**
		 * @brief Create a texture.
		 * @param Name.
		 * @param Path to texture.
		 * @param Filtering mode.
		 * @param Mip map level.
		 * @return Texture handle.
		 */
		Handle<Texture> create_texture(const std::string& name, const std::string& path, vk::Filter filtering, uint32_t mip_map_level = 1);

		/**
		 * Create a sky box.
		 * @param Name.
		 */
		Handle<SkyBox> create_sky_box(const std::string& name);

		/**
		 * Create a cube map.
		 * @param Name.
		 * @param Path to file containing top image.
		 * @param Path to file containing bottom image.
		 * @param Path to file containing north image.
		 * @param Path to file containing east image.
		 * @param Path to file containing south image.
		 * @param Path to file containing west image.
		 * @param Texture filtering.
		 */
		Handle<CubeMap> create_cube_map
		(
			const std::string& name,
			const std::string& top,
			const std::string& bottom,
			const std::string& north,
			const std::string& east,
			const std::string& south,
			const std::string& west,
			vk::Filter filter
		);

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

		/**
		 * Destroy a sky box.
		 * @param Sky box handle.
		 */
		void destroy(Handle<SkyBox> sky_box);

		/**
		 * Destroy a cube map.
		 * @param Cube map handle.
		 */
		void destroy(Handle<CubeMap> cube_map);

	private:
		
		/** Rendering engine. */
		ForwardRenderer* m_renderer;

		/** Mesh allocator. */
		std::unique_ptr<ResourceAllocator<Mesh>> m_mesh_allocator;

		/** Mesh map. */
		std::unordered_map<std::string, ResourceID> m_mesh_map;

		/** Shader allocator. */
		std::unique_ptr<ResourceAllocator<Shader>> m_shader_allocator;

		/** Shader map. */
		std::unordered_map<std::string, ResourceID> m_shader_map;

		/** Material allocator. */
		std::unique_ptr<ResourceAllocator<Material>> m_material_allocator;

		/** Material map. */
		std::unordered_map<std::string, ResourceID> m_material_map;

		/** Texture allocator. */
		std::unique_ptr<ResourceAllocator<Texture>> m_texture_allocator;

		/** Texture map. */
		std::unordered_map<std::string, ResourceID> m_texture_map;

		/** Sky box allocator. */
		std::unique_ptr<ResourceAllocator<SkyBox>> m_sky_box_allocator;

		/** Sky box map. */
		std::unordered_map<std::string, ResourceID> m_sky_box_map;

		/** Cube map allocator. */
		std::unique_ptr<ResourceAllocator<CubeMap>> m_cube_map_allocator;

		/** Cube map map. */
		std::unordered_map<std::string, ResourceID> m_cube_map_map;
	};
}