#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <engine.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>
#include <ecs\transform.hpp>
#include <components\mesh_renderer.hpp>

int main()
{
	dk::engine::initialize(4, "Test Window", 1280, 720);

	// Add systems
	dk::engine::scene.add_system<dk::TransformSystem>();
	dk::engine::scene.add_system<dk::MeshRendererSystem>();

	dk::engine::renderer.create_camera();

	auto shader = dk::engine::resource_manager.create_shader
	(
		dk::read_binary_file("shaders/standard.vert.spv"),
		dk::read_binary_file("shaders/standard.frag.spv")
	);

	auto material = dk::engine::resource_manager.create_material(shader);

	auto mesh = dk::engine::resource_manager.create_mesh
	(
		std::vector<uint16_t>
		{
			0, 1, 2,
			2, 3, 0
		},
		std::vector<dk::Vertex>
		{
			{ glm::vec3(-1, -1,  0), glm::vec2(0, 0) },
			{ glm::vec3( 1, -1,  0), glm::vec2(0, 0) },
			{ glm::vec3( 1,  1,  0), glm::vec2(0, 0) },
			{ glm::vec3(-1,  1,  0), glm::vec2(0, 0) }
		}
	);

	for(size_t i = 0; i < 200; ++i)
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(material);
		mesh_renderer->set_mesh(mesh);
	}

	dk::engine::simulate();
	dk::engine::shutdown();

	std::cin.get();
	return 0;
}