#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <config.hpp>
#include <engine.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>
#include <ecs\transform.hpp>
#include <components\camera.hpp>
#include <components\mesh_renderer.hpp>
#include <components\lights.hpp>
#include <components\rigidbody.hpp>
#include <components\character_controller.hpp>

class Player : public dk::Component<Player>
{
	friend class PlayerSystem;

public:

	DK_COMPONENT_BODY(Player)

private:

	dk::Handle<dk::Transform> m_transform;

	dk::Handle<dk::Transform> m_camera_transform;

	dk::Handle<dk::CharacterController> m_character_controller;

	float m_y_vel = 0.0f;

	float m_jump_timer = 0.0f;
};

class PlayerSystem : public dk::System<Player>
{
public:

	PlayerSystem(dk::Scene* scene) : dk::System<Player>(scene, 1)
	{
		dk::engine::input.register_axis("Horizontal", { { dk::KeyCode::A, -1.0f }, { dk::KeyCode::D, 1.0f }});
		dk::engine::input.register_axis("Vertical", { { dk::KeyCode::W, 1.0f }, { dk::KeyCode::S, -1.0f } });
		dk::engine::input.register_button("MouseLock", dk::KeyCode::M);
		dk::engine::input.register_button("Jump", dk::KeyCode::Space);
	 }

	~PlayerSystem() = default;

	void on_begin() override
	{
		auto component = get_component();
		component->m_transform = component->get_entity().get_component<dk::Transform>();
		component->m_camera_transform = component->m_transform->get_child(0);
		component->m_character_controller = component->get_entity().get_component<dk::CharacterController>();
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

		for (dk::Handle<Player> player : *this)
		{
			// Decrement jump timer
			player->m_jump_timer -= delta_time;

			// Check ground snap
			player->m_character_controller->set_ground_snap(player->m_jump_timer <= 0.0f);

			// Apply gravity
			if (player->m_character_controller->is_grounded() && player->m_jump_timer <= 0.0f)
			{
				// Jump
				if (dk::engine::input.get_button_down("Jump"))
				{
					player->m_y_vel = 5.0f;
					player->m_jump_timer = 0.3f;
				}
				else
					player->m_y_vel = 0.0f;
			}
			else
				player->m_y_vel -= delta_time * 9.8f;

			// Rotate body
			player->m_transform->mod_euler_angles(glm::vec3(0, m_vel.x * 0.08f, 0));

			// Rotate camera
			auto ea = player->m_camera_transform->get_local_euler_angles();
			ea += glm::vec3(m_vel.y * 0.08f, 0, 0);
			if (ea.x > 90.0f) ea.x = 90.0f;
			if (ea.x < -90.0f) ea.x = -90.0f;
			player->m_camera_transform->set_local_euler_angles(ea);
			
			// Move player
			glm::vec3 movement_vec = {};
			movement_vec += player->m_transform->get_forward() * y * 3.0f;
			movement_vec += player->m_transform->get_right() * x * 3.0f;
			movement_vec.y += player->m_y_vel;
			movement_vec *= delta_time;
			player->m_character_controller->move(movement_vec);
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

	// Physics systems
	dk::engine::scene.add_system<dk::TransformSystem>();
	dk::engine::scene.add_system<dk::RigidBodySystem>();
	dk::engine::scene.add_system<dk::CharacterControllerSystem>();

	// Gameplay systems
	dk::engine::scene.add_system<PlayerSystem>();

	// Rendering system
	dk::engine::scene.add_system<dk::CameraSystem>();
	dk::engine::scene.add_system<dk::DirectionalLightSystem>();
	dk::engine::scene.add_system<dk::PointLightSystem>();
	dk::engine::scene.add_system<dk::MeshRendererSystem>();

	// Player
	{
		dk::Entity entity1 = dk::Entity(&dk::engine::scene);

		dk::Handle<dk::Transform> transform1 = entity1.get_component<dk::Transform>();
		transform1->set_position(glm::vec3(0, 16, 1));

		entity1.add_component<dk::CharacterController>();

		// Camera
		{
			dk::Entity entity2 = dk::Entity(&dk::engine::scene);

			dk::Handle<dk::Camera> camera = entity2.add_component<dk::Camera>();
			dk::CameraSystem::set_main_camera(camera);

			dk::Handle<dk::Transform> transform2 = entity2.get_component<dk::Transform>();
			transform2->set_parent(transform1);
			transform2->set_local_position(glm::vec3(0, 0, 0));
		}

		entity1.add_component<Player>();
	}

	// Test bunny 1
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

	// Test bunny 2
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(dk::engine::resource_manager.get_material("standard.mat"));
		mesh_renderer->set_mesh(dk::engine::resource_manager.get_mesh("bunny.mesh"));

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0.1f, 4.0f, 0.2f));
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

	// Sloped floor
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);

		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(dk::engine::resource_manager.get_material("standard.mat"));
		mesh_renderer->set_mesh(dk::engine::resource_manager.get_mesh("cube.mesh"));

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0, 2.5f, 14));
		transform->set_local_scale(glm::vec3(16, 1, 16));
		transform->set_euler_angles(glm::vec3(-30.0f, 0, 0));

		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_box_shape(glm::vec3(16, 1, 16));
		rigid_body->set_static(true);
	}

	// Wall
	{
		dk::Entity entity = dk::Entity(&dk::engine::scene);

		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(dk::engine::resource_manager.get_material("standard.mat"));
		mesh_renderer->set_mesh(dk::engine::resource_manager.get_mesh("cube.mesh"));

		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0, 1, 0));
		transform->set_local_scale(glm::vec3(8, 4, 1));

		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_box_shape(glm::vec3(8, 4, 1));
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