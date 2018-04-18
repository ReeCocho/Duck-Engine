#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <engine.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>
#include <ecs\transform.hpp>
#include <components\camera.hpp>
#include <components\mesh_renderer.hpp>

class CameraController : public dk::Component<CameraController>
{
	friend class CameraControllerSystem;

public:

	CameraController() : dk::Component<CameraController>(dk::Handle<CameraController>(0, nullptr), dk::Entity()) {}

	CameraController(dk::Handle<CameraController> handle, dk::Entity entity) : Component<CameraController>(handle, entity) {}

private:

	dk::Handle<dk::Transform> m_transform;
};

class CameraControllerSystem : public dk::System<CameraController>
{
public:

	CameraControllerSystem(dk::Scene* scene) : dk::System<CameraController>(scene, 32) 
	{
		dk::engine::input.register_axis("Horizontal", { { dk::KeyCode::A, -1.0f }, { dk::KeyCode::D, 1.0f }});
		dk::engine::input.register_axis("Vertical", { { dk::KeyCode::W, 1.0f },{ dk::KeyCode::S, -1.0f } });
	 }

	~CameraControllerSystem() = default;

	void on_begin() override
	{
		auto component = get_component();
		component->m_transform = component->get_entity().get_component<dk::Transform>();
	}

	void on_late_tick(float delta_time) override
	{
		glm::vec2 m_vel = dk::engine::input.get_mouse_delta();
		float x = dk::engine::input.get_axis("Horizontal");
		float y = dk::engine::input.get_axis("Vertical");

		for (dk::Handle<CameraController> controller : *this)
		{
			auto ea = controller->m_transform->get_euler_angles();
			ea += glm::vec3(m_vel.y, m_vel.x, 0) * 0.08f;
			
			if (ea.x > 80.0f) ea.x = 80.0f;
			if (ea.x < -80.0f) ea.x = -80.0f;

			controller->m_transform->set_euler_angles(ea);
			controller->m_transform->mod_position(controller->m_transform->get_forward() * y * delta_time);
			controller->m_transform->mod_position(controller->m_transform->get_right() * x * delta_time);
		}
	}
};

int main()
{
	dk::engine::initialize(2, "Test Window", 1280, 720);

	// Add systems
	dk::engine::scene.add_system<dk::TransformSystem>();
	dk::engine::scene.add_system<CameraControllerSystem>();
	dk::engine::scene.add_system<dk::CameraSystem>();
	dk::engine::scene.add_system<dk::MeshRendererSystem>();

	auto shader = dk::engine::resource_manager.create_shader
	(
		dk::read_binary_file("shaders/standard.vert.spv"),
		dk::read_binary_file("shaders/standard.frag.spv")
	);

	auto texture = dk::engine::resource_manager.create_texture("textures/CoolCat.JPG", vk::Filter::eLinear);

	auto material = dk::engine::resource_manager.create_material(shader);
	material->set_texture(0, texture);

	auto mesh = dk::engine::resource_manager.create_mesh
	(
		std::vector<uint16_t>
		{
			0, 1, 2,
			2, 3, 0
		},
		std::vector<dk::Vertex>
		{
			{ glm::vec3(-1, -1,  0), glm::vec2(1, 1) },
			{ glm::vec3( 1, -1,  0), glm::vec2(0, 1) },
			{ glm::vec3( 1,  1,  0), glm::vec2(0, 0) },
			{ glm::vec3(-1,  1,  0), glm::vec2(1, 0) }
		}
	);

	// Camera
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);

		dk::Handle<dk::Camera> camera = entity.add_component<dk::Camera>();
		dk::CameraSystem::set_main_camera(camera);

		entity.add_component<CameraController>();

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(3, 0, 3));
		transform->set_euler_angles(glm::vec3(0, 225.0f, 0));
	}

	// Test quad
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