#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <engine.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>
#include <components\mesh_renderer.hpp>

std::unique_ptr<dk::Mesh> mesh;
std::unique_ptr<dk::Shader> shader;
std::unique_ptr<dk::Material> material;

int main()
{
	dk::engine::initialize(1, "Test Window", 1280, 720);

	// Add systems
	dk::engine::scene.add_system<dk::MeshRendererSystem>();

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
		std::vector<dk::Vertex>
		{
			{ glm::vec3(-1, -1, 0), glm::vec2(0, 0) },
			{ glm::vec3(1, -1,  0), glm::vec2(0, 0) },
			{ glm::vec3(1,  1,  0), glm::vec2(0, 0) },
			{ glm::vec3(-1,  1,  0), glm::vec2(0, 0) }
		});

	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(material.get());
		mesh_renderer->set_mesh(mesh.get());

		mesh_renderer->mvp = glm::perspective(
			glm::radians(80.0f),
			static_cast<float>(dk::engine::graphics.get_width()) / static_cast<float>(dk::engine::graphics.get_height()),
			0.01f,
			100.0f
		) *
		glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	}

	dk::engine::simulate();

	mesh->free();
	material->free();
	shader->free();

	dk::engine::shutdown();

	std::cin.get();
	return 0;
}