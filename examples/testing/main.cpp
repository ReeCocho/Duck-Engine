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
#include <components\lights.hpp>
#include <components\rigidbody.hpp>

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
		dk::engine::input.register_axis("Vertical", { { dk::KeyCode::W, 1.0f }, { dk::KeyCode::S, -1.0f } });
		dk::engine::input.register_button("MouseLock", dk::KeyCode::M);
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

		if (dk::engine::input.get_button_down("MouseLock"))
		{
			m_mouse_lock = !m_mouse_lock;
			dk::engine::input.set_locked_mouse(m_mouse_lock);
		}

		for (dk::Handle<CameraController> controller : *this)
		{
			auto ea = controller->m_transform->get_euler_angles();
			ea += glm::vec3(m_vel.y, m_vel.x, 0) * 0.08f;
			
			if (ea.x > 80.0f) ea.x = 80.0f;
			if (ea.x < -80.0f) ea.x = -80.0f;

			controller->m_transform->set_euler_angles(ea);
			controller->m_transform->mod_position(controller->m_transform->get_forward() * y * delta_time * 3.0f);
			controller->m_transform->mod_position(controller->m_transform->get_right() * x * delta_time * 3.0f);
		}
	}

private:

	bool m_mouse_lock = false;
};

struct MaterialData
{
	float metallic = 1.0f;
	char padding[12];

	float roughness = 1.0f;
} mat_data;

int main()
{
	// Initialize the engine
	dk::engine::initialize("./config.json");

	// Add systems
	dk::engine::scene.add_system<dk::TransformSystem>();
	dk::engine::scene.add_system<CameraControllerSystem>();
	dk::engine::scene.add_system<dk::CameraSystem>();
	dk::engine::scene.add_system<dk::RigidBodySystem>();
	dk::engine::scene.add_system<dk::DirectionalLightSystem>();
	dk::engine::scene.add_system<dk::PointLightSystem>();
	dk::engine::scene.add_system<dk::MeshRendererSystem>();

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

	// Test bunny
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(dk::engine::resource_manager.get_material("standard.mat"));
		mesh_renderer->set_mesh(dk::engine::resource_manager.get_mesh("bunny.mesh"));

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0.0f, 2.0f, 0.0f));
		transform->set_local_scale(glm::vec3(0.1f, 0.1f, 0.1f));
		transform->set_euler_angles(glm::vec3(75.0f, 11.0f, 13.0f));

		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_sphere_shape(0.5f);
	}

	// Floor
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);

		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(dk::engine::resource_manager.get_material("standard.mat"));
		mesh_renderer->set_mesh(dk::engine::resource_manager.get_mesh("cube.mesh"));

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0, -1, 0));
		transform->set_local_scale(glm::vec3(16, 1, 16));

		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_box_shape(glm::vec3(16, 1, 16));
		rigid_body->set_static(true);
	}

	// Directional light
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);
		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_euler_angles(glm::vec3(45, 45, 0));

		dk::Handle<dk::DirectionalLight> light = entity.add_component<dk::DirectionalLight>();
		light->set_color(glm::vec3(1, 1, 1));
		light->set_intensity(4.0f);
	}

	dk::engine::simulate();
	dk::engine::shutdown();

	std::cin.get();
	return 0;
}