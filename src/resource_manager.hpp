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
#include <graphics\material_shader.hpp>
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
		ResourceManager(ForwardRendererBase* renderer);

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
		ResourceAllocator<MaterialShader>& get_shader_allocator()
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
		Handle<MaterialShader> get_shader(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_shader_map.at(name); }
			catch (std::out_of_range e) { return Handle<MaterialShader>(0, nullptr); }
			return Handle<MaterialShader>(id, m_shader_allocator.get());
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
		 * Get mesh name.
		 * @param Mesh handle.
		 * @return Mesh name.
		 */
		std::string get_mesh_name(Handle<Mesh> mesh)
		{
			dk_assert(mesh.allocator == m_mesh_allocator.get());
			for (auto pair : m_mesh_map)
				if (pair.second == mesh.id)
					return pair.first;
			return "";
		}

		/**
		 * Get shader name.
		 * @param Shader handle.
		 * @return Shader name.
		 */
		std::string get_shader_name(Handle<MaterialShader> shader)
		{
			dk_assert(shader.allocator == m_shader_allocator.get());
			for (auto pair : m_shader_map)
				if (pair.second == shader.id)
					return pair.first;
			return "";
		}

		/**
		 * Get material name.
		 * @param Material handle.
		 * @return Material name.
		 */
		std::string get_material_name(Handle<Material> material)
		{
			dk_assert(material.allocator == m_material_allocator.get());
			for (auto pair : m_material_map)
				if (pair.second == material.id)
					return pair.first;
			return "";
		}

		/**
		 * Get texture name.
		 * @param Texture handle.
		 * @return Texture name.
		 */
		std::string get_texture_name(Handle<Texture> texture)
		{
			dk_assert(texture.allocator == m_texture_allocator.get());
			for (auto pair : m_texture_map)
				if (pair.second == texture.id)
					return pair.first;
			return "";
		}

		/**
		 * Get sky box name.
		 * @param Sky box handle.
		 * @return Sky box name.
		 */
		std::string get_sky_box_name(Handle<SkyBox> sky_box)
		{
			dk_assert(sky_box.allocator == m_sky_box_allocator.get());
			for (auto pair : m_sky_box_map)
				if (pair.second == sky_box.id)
					return pair.first;
			return "";
		}

		/**
		 * Get cube map name.
		 * @param Cube map handle.
		 * @return Cube map name.
		 */
		std::string get_cube_map_name(Handle<CubeMap> cube_map)
		{
			dk_assert(cube_map.allocator == m_cube_map_allocator.get());
			for (auto pair : m_cube_map_map)
				if (pair.second == cube_map.id)
					return pair.first;
			return "";
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
		Handle<MaterialShader> create_shader(const std::string& name, const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code, bool depth = true);

		/**
		 * @brief Create a material.
		 * @param Name.
		 * @param Shader handle.
		 * @return Material handle.
		 */
		Handle<Material> create_material(const std::string& name, Handle<MaterialShader> shader);

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
		 * @brief Create a texture
		 * @param Name.
		 * @param Image
		 * @param Image view.
		 * @param Sampler.
		 * @param Image memory.
		 * @param Filtering.
		 * @param Width.
		 * @param Height.
		 * @param Mip map levels.
		 */
		Handle<Texture> create_texture
		(
			const std::string& name,
			vk::Image image,
			vk::ImageView image_view,
			vk::Sampler sampler,
			vk::DeviceMemory memory,
			vk::Filter filter,
			uint32_t width,
			uint32_t height,
			uint32_t mip_map_levels = 1
		);

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
		void destroy(Handle<MaterialShader> shader);

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
		ForwardRendererBase* m_renderer;

		/** Mesh allocator. */
		std::unique_ptr<ResourceAllocator<Mesh>> m_mesh_allocator;

		/** Mesh map. */
		std::unordered_map<std::string, ResourceID> m_mesh_map;

		/** Shader allocator. */
		std::unique_ptr<ResourceAllocator<MaterialShader>> m_shader_allocator;

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