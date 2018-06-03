#include <engine/common.hpp>
#include <fstream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <utilities\common.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>
#include <components\transform.hpp>
#include <components\camera.hpp>
#include <components\mesh_renderer.hpp>
#include <components\lights.hpp>
#include <components\rigidbody.hpp>
#include <components\character_controller.hpp>


#if DK_EDITOR
namespace active_system = dk::editor;
#else
namespace active_system = dk::engine;
#endif


class Player : public dk::Component<Player>
{
	friend class PlayerSystem;

public:

	DK_COMPONENT_BODY(Player)

private:

	dk::Handle<dk::Transform> m_transform;

	dk::Handle<dk::Transform> m_camera_transform;

	dk::Handle<dk::CharacterController> m_character_controller;

	float m_jump_timer = 0.0f;

	float m_movement_timer = 0.0f;

	glm::vec3 m_velocity = {};

	glm::vec3 m_cam_start_pos = {};

	glm::vec3 m_cam_cur_pos = {};
};

class PlayerSystem : public dk::System<Player>
{
public:

	PlayerSystem(dk::Scene* scene) : dk::System<Player>(scene, false, 1)
	{
		active_system::input.register_axis("Horizontal", { { dk::KeyCode::A, -1.0f }, { dk::KeyCode::D, 1.0f }});
		active_system::input.register_axis("Vertical", { { dk::KeyCode::W, 1.0f }, { dk::KeyCode::S, -1.0f } });
		active_system::input.register_button("MouseLock", dk::KeyCode::M);
		active_system::input.register_button("Jump", dk::KeyCode::Space);
	 }

	~PlayerSystem() = default;

	void on_begin() override
	{
		auto component = get_component();
		component->m_transform = component->get_entity().get_component<dk::Transform>();
		component->m_camera_transform = component->m_transform->get_child(0);
		component->m_character_controller = component->get_entity().get_component<dk::CharacterController>();
		component->m_cam_start_pos = component->m_camera_transform->get_local_position();
		component->m_cam_cur_pos = component->m_cam_start_pos;
	}

	void on_late_tick(float delta_time) override
	{
		glm::vec2 m_vel = active_system::input.get_mouse_delta();
		const float x = active_system::input.get_axis("Horizontal");
		const float y = active_system::input.get_axis("Vertical");

		// Toggle mouse lock
		if (active_system::input.get_button_down("MouseLock"))
		{
			m_mouse_lock = !m_mouse_lock;
			active_system::input.set_locked_mouse(m_mouse_lock);
		}

		for (dk::Handle<Player> player : *this)
		{
			// Decrement jump timer
			player->m_jump_timer -= delta_time;

			// Check ground snap
			player->m_character_controller->set_ground_snap(player->m_jump_timer <= 0.0f);

			// Compute movement vector
			glm::vec3 movement_vec = {};
			movement_vec += player->m_transform->get_forward() * y;
			movement_vec += player->m_transform->get_right() * x;

			// Normalize movement vector
			if (movement_vec.x != 0.0f && movement_vec.y != 0.0f)
				movement_vec = glm::normalize(movement_vec);

			// If we are moving do head bob
			if (movement_vec.x != 0.0f || movement_vec.y != 0.0f)
			{
				player->m_movement_timer += delta_time * m_head_bob_rate;
				player->m_cam_cur_pos = player->m_cam_start_pos;
				player->m_cam_cur_pos.x += glm::cos(player->m_movement_timer) * m_head_bob;
				player->m_cam_cur_pos.y += glm::sin(player->m_movement_timer * 2.0f) * m_head_bob;
			}
			else
			{
				player->m_cam_cur_pos = glm::mix(player->m_cam_cur_pos, player->m_cam_start_pos, delta_time * m_head_return_rate);
			}

			// Move camera position
			player->m_camera_transform->set_local_position(player->m_cam_cur_pos);

			// Movement
			player->m_velocity.x = movement_vec.x * m_speed;
			player->m_velocity.z = movement_vec.z * m_speed;

			// Movement logic
			if (player->m_character_controller->is_grounded() && player->m_jump_timer <= 0.0f)
			{
				// Jump
				if (active_system::input.get_button_down("Jump"))
				{
					player->m_velocity.y = 5.0f;
					player->m_jump_timer = 0.3f;
				}
				else player->m_velocity.y = 0.0f;
			}
			else
			{
				// Gravity
				player->m_velocity.y -= delta_time * 9.8f;
			}

			// Rotate body
			player->m_transform->mod_euler_angles(glm::vec3(0, m_vel.x * 0.08f, 0));

			// Rotate camera
			auto ea = player->m_camera_transform->get_local_euler_angles();
			ea += glm::vec3(m_vel.y * 0.08f, 0, 0);
			if (ea.x > 90.0f) ea.x = 90.0f;
			if (ea.x < -90.0f) ea.x = -90.0f;
			player->m_camera_transform->set_local_euler_angles(ea);
			
			// Move player
			player->m_character_controller->move(player->m_velocity * delta_time);
		}
	}

	void serialize(dk::ReflectionContext& context)
	{
		auto& archive = static_cast<dk::ComponentArchive&>(context);
		archive.set_name("Player");
	}

private:

	bool m_mouse_lock = false;

	const float m_speed = 6.0f;

	const float m_head_bob = 0.025f;

	const float m_head_bob_rate = 10.0f;

	const float m_head_return_rate = 3.0f;
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
	active_system::initialize("./config.json");
	
	// Add systems
	
	// Physics systems
	active_system::scene.add_system<dk::TransformSystem>();
	active_system::scene.add_system<dk::RigidBodySystem>();
	active_system::scene.add_system<dk::CharacterControllerSystem>();
	
	// Gameplay systems
	active_system::scene.add_system<PlayerSystem>();
	
	// Rendering system
	active_system::scene.add_system<dk::CameraSystem>();
	active_system::scene.add_system<dk::DirectionalLightSystem>();
	active_system::scene.add_system<dk::PointLightSystem>();
	active_system::scene.add_system<dk::MeshRendererSystem>();
	
	// Player
	{
		dk::Entity entity1 = dk::Entity(&active_system::scene);
	
		dk::Handle<dk::Transform> transform1 = entity1.get_component<dk::Transform>();
		transform1->set_position(glm::vec3(0, 16, 1));
	
		entity1.add_component<dk::CharacterController>();
	
		// Camera
		{
			dk::Entity entity2 = dk::Entity(&active_system::scene);
	
			dk::Handle<dk::Camera> camera = entity2.add_component<dk::Camera>();
			camera->set_sky_box(active_system::resource_manager.get_sky_box("sky.sky"));
			dk::CameraSystem::set_main_camera(camera);
	
			dk::Handle<dk::Transform> transform2 = entity2.get_component<dk::Transform>();
			transform2->set_parent(transform1);
			transform2->set_local_position(glm::vec3(0, 1, 0));
		}
	
		entity1.add_component<Player>();
	}
	
	// Test sphere 1
	{
		dk::Entity entity = dk::Entity(&active_system::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(active_system::resource_manager.get_material("metal.mat"));
		mesh_renderer->set_mesh(active_system::resource_manager.get_mesh("sphere.mesh"));
	
		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0.0f, 2.0f, 0.0f));
		transform->set_euler_angles(glm::vec3(75.0f, 11.0f, 13.0f));
	
		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_sphere_shape(0.5f);
	}
	
	// Test sphere 2
	{
		dk::Entity entity = dk::Entity(&active_system::scene);
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(active_system::resource_manager.get_material("mud.mat"));
		mesh_renderer->set_mesh(active_system::resource_manager.get_mesh("sphere.mesh"));
	
		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0.1f, 4.0f, 0.2f));
		transform->set_euler_angles(glm::vec3(75.0f, 11.0f, 13.0f));
	
		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_sphere_shape(0.5f);
	}
	
	// Floor
	{
		dk::Entity entity = dk::Entity(&active_system::scene);
	
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(active_system::resource_manager.get_material("mud.mat"));
		mesh_renderer->set_mesh(active_system::resource_manager.get_mesh("cube.mesh"));
	
		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0, -1, 0));
		transform->set_local_scale(glm::vec3(16, 1, 16));
	
		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_box_shape(glm::vec3(16, 1, 16));
		rigid_body->set_static(true);
	}
	
	// Sloped floor
	{
		dk::Entity entity = dk::Entity(&active_system::scene);
	
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(active_system::resource_manager.get_material("metal.mat"));
		mesh_renderer->set_mesh(active_system::resource_manager.get_mesh("cube.mesh"));
	
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
		dk::Entity entity = dk::Entity(&active_system::scene);
	
		dk::Handle<dk::MeshRenderer> mesh_renderer = entity.add_component<dk::MeshRenderer>();
		mesh_renderer->set_material(active_system::resource_manager.get_material("metal.mat"));
		mesh_renderer->set_mesh(active_system::resource_manager.get_mesh("cube.mesh"));
	
		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_position(glm::vec3(0, 1, 0));
		transform->set_local_scale(glm::vec3(8, 4, 1));
	
		dk::Handle<dk::RigidBody> rigid_body = entity.add_component<dk::RigidBody>();
		rigid_body->set_box_shape(glm::vec3(8, 4, 1));
		rigid_body->set_static(true);
	}
	
	// Directional light
	{
		dk::Entity entity = dk::Entity(&active_system::scene);
		dk::Handle<dk::Transform> transform = entity.get_component<dk::Transform>();
		transform->set_euler_angles(glm::vec3(45, 145, 0));
	
		dk::Handle<dk::DirectionalLight> light = entity.add_component<dk::DirectionalLight>();
		light->set_color(glm::vec3(1, 1, 1));
		light->set_intensity(4.0f);
	}
	
	dk::Archive archive(512);
	
	// Loop over every system
	for (size_t i = 0; i < active_system::scene.get_system_count(); ++i)
	{
		dk::SystemBase* system = active_system::scene.get_system_by_index(i);
		auto components = system->get_active_components();
	
		// Write component count
		archive.write<uint32_t>(static_cast<uint32_t>(components.size()));
	
		// Write every component
		for (auto component : components)
		{
			dk::ComponentArchive comp_archive(&active_system::scene, &active_system::resource_manager, &archive);
			system->serialize_by_id(comp_archive, component);
		}
	}
	
	std::ofstream stream("./test.dat");
	stream.write(archive.get_data(), archive.get_bytes_allocated());
	stream.close();
	
	active_system::simulate();
	active_system::shutdown();

	std::cin.get();
	return 0;
}