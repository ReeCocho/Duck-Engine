#pragma once

/**
 * @file shader.hpp
 * @brief Vulkan shaders.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "shader.hpp"

namespace dk
{
	/**
	 * Standard data sent to a MaterialShader's vertex shader.
	 */
	struct VertexShaderData
	{
		glm::mat4 model;
		glm::mat4 mvp;
	};

	/**
	 * Standard data sent to a MaterialShader's fragment shader.
	 */
	struct FragmentShaderData
	{
		int unused;
	};

	/**
	 * Information for creating a material shader.
	 */
	struct MaterialShaderCreateInfo
	{
		/** Render pass. */
		vk::RenderPass render_pass = {};

		/** Descriptor set layouts. */
		std::vector<vk::DescriptorSetLayout> descriptor_set_layouts = {};

		/** Stage flags. */
		vk::ShaderStageFlags stage_flags = vk::ShaderStageFlagBits(0);

		/** Depth testing. */
		bool depth_test = true;

		/** Depth comparison operator. */
		vk::CompareOp depth_compare = vk::CompareOp(0);

		/** Depth write. */
		bool depth_write = true;
	};

	/**
	 * @brief Material shader object.
	 */
	class MaterialShader : public Shader
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		MaterialShader();

		/**
		 * @brief Constructor.
		 * @param Graphics context the shader is bound to.
		 * @param Material shader create infos.
		 * @param Vertex shader byte code.
		 * @param Fragment shader byte code.
		 */
		MaterialShader
		(
			Graphics* graphics, 
			std::vector<MaterialShaderCreateInfo>& create_info,
			const std::vector<char>& vert_byte_code, 
			const std::vector<char>& frag_byte_code
		);

		/**
		 * @brief Destructor.
		 */
		~MaterialShader() = default;

		/**
		 * @brief Free memory used by the shader.
		 * @note Used internally. Do not call.
		 */
		void free() override;

		/**
		 * @brief Get size in bytes of the vertex uniform buffer.
		 * @return Size of vertex uniform buffer.
		 */
		size_t get_vertex_buffer_size() const
		{
			return m_vertex_buffer_size;
		}

		/**
		 * @brief Get size in bytes of the per instance vertex uniform buffer.
		 * @return Size of the per instance vertex uniform buffer.
		 */
		size_t get_inst_vertex_buffer_size() const
		{
			return m_inst_vertex_buffer_size;
		}

		/**
		 * @brief Get size in bytes of the fragment uniform buffer.
		 * @return Size of fragment uniform buffer.
		 */
		size_t get_fragment_buffer_size() const
		{
			return m_fragment_buffer_size;
		}

		/**
		 * @brief Get size in bytes of the per instance fragment uniform buffer.
		 * @return Size of the per instance fragment uniform buffer.
		 */
		size_t get_inst_fragment_buffer_size() const
		{
			return m_inst_fragment_buffer_size;
		}

		/**
		 * @brief Get descriptor set layout.
		 * @return Descriptor set layout.
		 */
		vk::DescriptorSetLayout& get_descriptor_set_layout()
		{
			return m_vk_descriptor_set_layout;
		}

		/**
		 * @brief Get texture descriptor set layout.
		 * @return Texture descriptor set layout.
		 */
		vk::DescriptorSetLayout& get_texture_descriptor_set_layout()
		{
			return m_vk_texture_descriptor_set_layout;
		}

	private:

		/** Descriptor set layout. */
		vk::DescriptorSetLayout m_vk_descriptor_set_layout;

		/** Texture descriptor set layout. */
		vk::DescriptorSetLayout m_vk_texture_descriptor_set_layout;

		/** Size in bytes of the vertex uniform buffer. */
		size_t m_vertex_buffer_size;

		/** Size in bytes of the per instance vertex uniform buffer. */
		size_t m_inst_vertex_buffer_size;

		/** Size in bytes of the fragment uniform buffer. */
		size_t m_fragment_buffer_size;

		/** Size in bytes of the per instance fragment uniform buffer. */
		size_t m_inst_fragment_buffer_size;
	};

	/** Handle to a material shader. */
	using HMaterialShader = Handle<MaterialShader>;
}