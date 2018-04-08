#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <graphics\graphics.hpp>
#include <graphics\forward_renderer.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>

struct VertexShaderData
{
	glm::mat4 mvp;
};

struct FragmentShaderData
{
	int unused;
};

class MeshRenderer
{
public:

	MeshRenderer(dk::Renderer& renderer, dk::Material& material, dk::Mesh& mesh) :
		m_renderer(renderer),
		m_material(material),
		m_mesh(mesh),
		m_command_buffer(m_renderer.get_graphics().get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary))
	{
		// Create buffers
		m_vertex_uniform_buffer = m_renderer.get_graphics().create_buffer
		(
			m_material.get_shader().get_inst_vertex_buffer_size(),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		m_fragment_uniform_buffer = m_renderer.get_graphics().create_buffer
		(
			m_material.get_shader().get_inst_fragment_buffer_size(),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		vk::DescriptorPoolSize pool_size = {};
		pool_size.type = vk::DescriptorType::eUniformBuffer;
		pool_size.descriptorCount = 4;

		vk::DescriptorPoolCreateInfo pool_info = {};
		pool_info.poolSizeCount = 1;
		pool_info.pPoolSizes = &pool_size;
		pool_info.maxSets = 1;

		m_vk_descriptor_pool = m_renderer.get_graphics().get_logical_device().createDescriptorPool(pool_info);
		dk_assert(m_vk_descriptor_pool);

		vk::DescriptorSetAllocateInfo alloc_info = {};
		alloc_info.descriptorPool = m_vk_descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &m_material.get_shader().get_descriptor_set_layout();

		m_vk_descriptor_set = m_renderer.get_graphics().get_logical_device().allocateDescriptorSets(alloc_info)[0];
		dk_assert(m_vk_descriptor_set);

		std::array<vk::DescriptorBufferInfo, 4> buffer_infos = {};
		buffer_infos[0].buffer = m_material.get_vertex_uniform_buffer().buffer;
		buffer_infos[0].offset = 0;
		buffer_infos[0].range = m_material.get_shader().get_vertex_buffer_size();

		buffer_infos[1].buffer = m_vertex_uniform_buffer.buffer;
		buffer_infos[1].offset = 0;
		buffer_infos[1].range = m_material.get_shader().get_inst_vertex_buffer_size();

		buffer_infos[2].buffer = m_material.get_fragment_uniform_buffer().buffer;
		buffer_infos[2].offset = 0;
		buffer_infos[2].range = m_material.get_shader().get_fragment_buffer_size();

		buffer_infos[3].buffer = m_fragment_uniform_buffer.buffer;
		buffer_infos[3].offset = 0;
		buffer_infos[3].range = m_material.get_shader().get_inst_fragment_buffer_size();

		std::array<vk::WriteDescriptorSet, 4> writes = {};
		
		for (size_t i = 0; i < 4; ++i)
		{
			writes[i].dstSet = m_vk_descriptor_set;
			writes[i].dstBinding = static_cast<uint32_t>(i);
			writes[i].dstArrayElement = 0;
			writes[i].descriptorType = vk::DescriptorType::eUniformBuffer;
			writes[i].descriptorCount = 1;
			writes[i].pBufferInfo = &buffer_infos[i];
		}

		m_renderer.get_graphics().get_logical_device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}

	~MeshRenderer()
	{
		m_renderer.get_graphics().get_logical_device().destroyDescriptorPool(m_vk_descriptor_pool);
		m_vertex_uniform_buffer.free(m_renderer.get_graphics().get_logical_device());
		m_fragment_uniform_buffer.free(m_renderer.get_graphics().get_logical_device());
		m_command_buffer.free();
	}

	void draw()
	{
		{
			VertexShaderData v_data = {};
			v_data.mvp = mvp;

			void* data = m_renderer.get_graphics().get_logical_device().mapMemory(m_vertex_uniform_buffer.memory, 0, sizeof(VertexShaderData));
			memcpy(data, &v_data, sizeof(VertexShaderData));
			m_renderer.get_graphics().get_logical_device().unmapMemory(m_vertex_uniform_buffer.memory);
		}

		{
			FragmentShaderData f_data = {};

			void* data = m_renderer.get_graphics().get_logical_device().mapMemory(m_fragment_uniform_buffer.memory, 0, sizeof(FragmentShaderData));
			memcpy(data, &f_data, sizeof(FragmentShaderData));
			m_renderer.get_graphics().get_logical_device().unmapMemory(m_fragment_uniform_buffer.memory);
		}

		dk::RenderableObject renderable =
		{
			m_command_buffer,
			&m_material.get_shader(),
			&m_mesh,
			{ m_vk_descriptor_set }
		};
		m_renderer.draw(renderable);
	}

	glm::mat4 mvp;

private:

	dk::Renderer& m_renderer;

	dk::Material& m_material;

	dk::Mesh& m_mesh;

	dk::VkManagedCommandBuffer m_command_buffer;

	vk::DescriptorPool m_vk_descriptor_pool;

	vk::DescriptorSet m_vk_descriptor_set;

	dk::VkMemBuffer m_vertex_uniform_buffer;

	dk::VkMemBuffer m_fragment_uniform_buffer;
};

int main()
{
	{
		dk::Graphics graphics(1, "Test window", 1280, 720);
		dk::ForwardRenderer renderer(graphics);
		bool running = true;

		dk::Shader shader(	graphics, 
							renderer.get_shader_render_pass(), 
							dk::read_binary_file("shaders/standard.vert.spv"),
							dk::read_binary_file("shaders/standard.frag.spv"));
		
		dk::Material material(graphics, shader);
		
		dk::Mesh mesh(graphics,
		{
			0, 1, 2,
			2, 3, 0
		},
		{
			{ glm::vec3(-1, -1,  0), glm::vec2(0, 0) },
			{ glm::vec3( 1, -1,  0), glm::vec2(0, 0) },
			{ glm::vec3( 1,  1,  0), glm::vec2(0, 0) },
			{ glm::vec3(-1,  1,  0), glm::vec2(0, 0) }
		});
		
		glm::mat4 mvp = {};
		mvp = glm::perspective
		(
			glm::radians(80.0f),
			static_cast<float>(graphics.get_width()) / static_cast<float>(graphics.get_height()),
			0.01f,
			100.0f
		) *
		glm::lookAt
		(
			glm::vec3(0, 0, 3),
			glm::vec3(0, 0, 0), 
			glm::vec3(0, 1, 0)
		);
		
		MeshRenderer mesh_renderer(renderer, material, mesh);
		mesh_renderer.mvp = mvp;

		while (running)
		{
			// Loop through events
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				if (evt.type == SDL_QUIT)
					running = false;
			}

			mesh_renderer.draw();
			renderer.render();
		}

		// Wait for presentation to finish
		graphics.get_device_manager().get_present_queue().waitIdle();

		mesh.free();
		material.free();
		shader.free();
	}

	std::cin.get();
	return 0;
}