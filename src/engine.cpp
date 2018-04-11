/**
 * @file engine.cpp
 * @brief Engine source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <utilities\threading.hpp>
#include <graphics\material.hpp>
#include "engine.hpp"

namespace
{
	class MeshRenderer
	{
	public:

		MeshRenderer(dk::engine::RendererType& renderer, dk::Material& material, dk::Mesh& mesh) :
			m_renderer(renderer),
			m_material(material),
			m_mesh(mesh),
			
		{

		}

		~MeshRenderer()
		{

		}

		void draw()
		{

		}

	private:

		dk::engine::RendererType& m_renderer;

		dk::Material& m_material;

		dk::Mesh& m_mesh;

		dk::VkManagedCommandBuffer m_command_buffer;

		vk::DescriptorPool m_vk_descriptor_pool;

		vk::DescriptorSet m_vk_descriptor_set;

		dk::VkMemBuffer m_vertex_uniform_buffer;

		dk::VkMemBuffer m_fragment_uniform_buffer;
	};



	/** Rendering thread. */
	std::unique_ptr<dk::SimulationThread> rendering_thread;

	std::unique_ptr<dk::Mesh> mesh;
	std::unique_ptr<dk::Shader> shader;
	std::unique_ptr<dk::Material> material;
	std::vector<std::unique_ptr<MeshRenderer>> mesh_renderers;
}

namespace dk
{
	namespace engine
	{
		Graphics graphics = {};

		RendererType renderer = {};

		Input input;

		Scene scene = {};



		void initialize(size_t thread_count, const std::string& name, int width, int height)
		{
			::new(&graphics)(Graphics)(thread_count, name, width, height);
			::new(&renderer)(RendererType)(&graphics);
			input = Input();
			scene = {};

			rendering_thread = std::make_unique<SimulationThread>([]() { renderer.render(); });
		}

		void simulate()
		{
			shader = std::make_unique<dk::Shader>(	
					dk::engine::graphics,
					dk::engine::renderer.get_shader_render_pass(), 
					dk::read_binary_file("shaders/standard.vert.spv"),
					dk::read_binary_file("shaders/standard.frag.spv"));
			
			material = std::make_unique<dk::Material>(dk::engine::graphics, *shader.get());
			
			mesh = std::make_unique<dk::Mesh>(
				dk::engine::graphics,
				std::vector<uint16_t>
				{
					0, 1, 2,
					2, 3, 0
				},
				std::vector<Vertex>
				{
					{ glm::vec3(-1, -1,  0), glm::vec2(0, 0) },
					{ glm::vec3( 1, -1,  0), glm::vec2(0, 0) },
					{ glm::vec3( 1,  1,  0), glm::vec2(0, 0) },
					{ glm::vec3(-1,  1,  0), glm::vec2(0, 0) }
				});
			
			mesh_renderers.resize(100);
			for (size_t i = 0; i < mesh_renderers.size(); ++i)
			{
				auto& mesh_renderer = mesh_renderers[i];

				glm::mat4 mvp = {};
				mvp = glm::perspective
				(
					glm::radians(80.0f),
					static_cast<float>(dk::engine::graphics.get_width()) / static_cast<float>(dk::engine::graphics.get_height()),
					0.01f,
					100.0f
				) *
				glm::lookAt
				(
					glm::vec3(0, 0, 3),
					glm::vec3(0, 0, 0),
					glm::vec3(0, 1, 0)
				);

				mvp = glm::translate(mvp, glm::vec3(static_cast<float>(i * 2) - 10, 0, 0));

				mesh_renderer = std::make_unique<MeshRenderer>(dk::engine::renderer, *material.get(), *mesh.get());
				mesh_renderer->mvp = mvp;
			}

			while (!input.is_closing())
			{
				// Gather input
				input.poll_events();

				// Wait for threads to finish
				rendering_thread->wait();
				
				// Perform a game tick
				scene.tick(0);
				
				for (auto& mesh_renderer : mesh_renderers)
					mesh_renderer->draw();

				// Start rendering thread
				rendering_thread->start();
			}
		}

		void shutdown()
		{
			// Stop threads
			rendering_thread.reset();

			// Wait for presentation to finish
			graphics.get_device_manager().get_present_queue().waitIdle();

			for (auto& mesh_renderer : mesh_renderers)
				mesh_renderer.reset();

			mesh->free();
			material->free();
			shader->free();

			scene.shutdown();
			renderer.shutdown();
			graphics.shutdown();
		}
	}
}