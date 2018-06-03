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
		HMesh get_mesh(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_mesh_map.at(name); }
			catch (std::out_of_range e) { return HMesh(0, nullptr); }
			return HMesh(id, m_mesh_allocator.get());
		}

		/**
		 * @brief Get a shader.
		 * @param Name.
		 * @param Shader handle.
		 */
		HMaterialShader get_shader(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_shader_map.at(name); }
			catch (std::out_of_range e) { return HMaterialShader(0, nullptr); }
			return HMaterialShader(id, m_shader_allocator.get());
		}

		/**
		 * @brief Get a material.
		 * @param Name.
		 * @param Material handle.
		 */
		HMaterial get_material(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_material_map.at(name); }
			catch (std::out_of_range e) { return HMaterial(0, nullptr); }
			return HMaterial(id, m_material_allocator.get());
		}

		/**
		 * @brief Get a texture.
		 * @param Name.
		 * @param Texture handle.
		 */
		HTexture get_texture(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_texture_map.at(name); }
			catch (std::out_of_range e) { return HTexture(0, nullptr); }
			return HTexture(id, m_texture_allocator.get());
		}

		/**
		 * @brief Get a sky box.
		 * @param Name.
		 * @param Sky box handle.
		 */
		HSkyBox get_sky_box(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_sky_box_map.at(name); }
			catch (std::out_of_range e) { return HSkyBox(0, nullptr); }
			return HSkyBox(id, m_sky_box_allocator.get());
		}

		/**
		 * @brief Get a cube map.
		 * @param Name.
		 * @param Cube map handle.
		 */
		HCubeMap get_cube_map(const std::string& name)
		{
			ResourceID id = 0;
			try { id = m_cube_map_map.at(name); }
			catch (std::out_of_range e) { return HCubeMap(0, nullptr); }
			return HCubeMap(id, m_cube_map_allocator.get());
		}

		/**
		 * Get mesh name.
		 * @param Mesh handle.
		 * @return Mesh name.
		 */
		std::string get_mesh_name(HMesh mesh)
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
		std::string get_shader_name(HMaterialShader shader)
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
		std::string get_material_name(HMaterial material)
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
		std::string get_texture_name(HTexture texture)
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
		std::string get_sky_box_name(HSkyBox sky_box)
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
		std::string get_cube_map_name(HCubeMap cube_map)
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
		HMesh create_mesh(const std::string& name, const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices);

		/**
		 * @brief Create a mesh.
		 * @param Name.
		 * @param Path to the OBJ file.
		 * @return Mesh handle.
		 */
		HMesh create_mesh(const std::string& name, const std::string& path);

		/**
		 * @brief Create a shader.
		 * @param Name.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 * @param Should the shader perform depth testing?
		 * @return Shader handle.
		 */
		HMaterialShader create_shader(const std::string& name, const std::vector<char>& vert_byte_code, const std::vector<char>& frag_byte_code, bool depth = true);

		/**
		 * @brief Create a material.
		 * @param Name.
		 * @param Shader handle.
		 * @return Material handle.
		 */
		HMaterial create_material(const std::string& name, HMaterialShader shader);

		/**
		 * @brief Create a texture.
		 * @param Name.
		 * @param Path to texture.
		 * @param Filtering mode.
		 * @param Mip map level.
		 * @return Texture handle.
		 */
		HTexture create_texture(const std::string& name, const std::string& path, vk::Filter filtering, uint32_t mip_map_level = 1);

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
		HTexture create_texture
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
		HSkyBox create_sky_box(const std::string& name);

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
		HCubeMap create_cube_map
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
		void destroy(HMesh mesh);

		/**
		 * @brief Destroy a shader.
		 * @param Shader handle.
		 */
		void destroy(HMaterialShader shader);

		/**
		 * @brief Destroy a material.
		 * @param Material handle.
		 */
		void destroy(HMaterial material);

		/**
		 * @brief Destroy a texture.
		 * @param Texture handle.
		 */
		void destroy(HTexture texture);

		/**
		 * Destroy a sky box.
		 * @param Sky box handle.
		 */
		void destroy(HSkyBox sky_box);

		/**
		 * Destroy a cube map.
		 * @param Cube map handle.
		 */
		void destroy(HCubeMap cube_map);

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