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
	dk::engine::initialize(4, "Test Window", 1280, 720);

	// Add systems
	dk::engine::scene.add_system<dk::TransformSystem>();
	dk::engine::scene.add_system<CameraControllerSystem>();
	dk::engine::scene.add_system<dk::CameraSystem>();
	dk::engine::scene.add_system<dk::DirectionalLightSystem>();
	dk::engine::scene.add_system<dk::PointLightSystem>();
	dk::engine::scene.add_system<dk::MeshRendererSystem>();

	auto shader = dk::engine::resource_manager.create_shader
	(
		"standard",
		dk::read_binary_file("shaders/standard.vert.spv"),
		dk::read_binary_file("shaders/standard.frag.spv")
	);

	auto metal_albedo = dk::engine::resource_manager.create_texture("textures/Metal_Albedo.png", vk::Filter::eLinear);
	auto metal_normal = dk::engine::resource_manager.create_texture("textures/Metal_Normal.png", vk::Filter::eLinear);
	auto metal_metallic = dk::engine::resource_manager.create_texture("textures/Metal_Metallic.png", vk::Filter::eLinear);
	auto metal_roughness = dk::engine::resource_manager.create_texture("textures/Metal_Roughness.png", vk::Filter::eLinear);
	auto metal_ao = dk::engine::resource_manager.create_texture("textures/Metal_AO.png", vk::Filter::eLinear);

	auto white = dk::engine::resource_manager.create_texture("textures/white.png", vk::Filter::eNearest);

	auto material = dk::engine::resource_manager.create_material("standard", shader);
	material->set_texture(0, metal_albedo);
	material->set_texture(1, metal_normal);
	material->set_texture(2, metal_metallic);
	material->set_texture(3, metal_roughness);
	material->set_texture(4, metal_ao);
	material->set_fragment_data<MaterialData>(mat_data);

	auto sphere = dk::engine::resource_manager.create_mesh("./meshes/sphere.obj");
	auto cube = dk::engine::resource_manager.create_mesh("./meshes/cube.obj");

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

	// Test sphere
	for(float i = 0.0f; i < 40.0f; i += 1.0f)
		for(float k = 0.0f; k < 40.0f; k += 1.0f)
		{
			dk::Entity entity = dk::Entity(&dk::engine::scene);
			dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
			mesh_renderer->set_material(material);
			mesh_renderer->set_mesh(sphere);

			dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
			transform->set_position(glm::vec3(i, 2.0f, k));
		}

	// Floor
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(material);
		mesh_renderer->set_mesh(cube);

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0, -1, 0));
		transform->set_local_scale(glm::vec3(16, 1, 16));
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

	// Point lights
	for (float i = 0.0f; i < 40.0f; i += 4.0f)
		for (float k = 0.0f; k < 40.0f; k += 4.0f)
		{
			dk::Entity entity = dk::Entity(&dk::engine::scene);
			dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
			transform->set_position(glm::vec3(i, 2, k));

			dk::Handle<dk::PointLight> light = entity.add_component<dk::PointLight>();
			light->set_color(glm::vec3(1, 0, 0));
			light->set_intensity(64.0f);
			light->set_range(4.0f);
		}

	dk::engine::simulate();
	dk::engine::shutdown();

	std::cin.get();
	return 0;
}