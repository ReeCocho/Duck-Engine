/** Includes. */
#include <ecs\transform.hpp>
#include <engine.hpp>
#include "character_controller.hpp"

namespace dk
{
    void CharacterControllerSystem::on_begin()
    {
        Handle<CharacterController> controller = get_component();

        // Get transform component
		controller->m_transform = controller->get_entity().get_component<Transform>();
		auto pos = controller->m_transform->get_position();
		auto rot = controller->m_transform->get_rotation();

		// Set physics transform
		btTransform transform = {};
		transform.setIdentity();
		transform.setOrigin({ pos.x, pos.y, pos.z });
		transform.setRotation({ rot.x, rot.y, rot.z, rot.w });

		// Create motion state
		controller->m_motion_state = std::make_unique<btDefaultMotionState>(transform);

		// Create shape
		controller->m_shape = std::make_unique<btEmptyShape>();

		btRigidBody::btRigidBodyConstructionInfo info
		(
			1.0f,
			controller->m_motion_state.get(),
			controller->m_shape.get()
		);

		// Create rigid body
		controller->m_rigid_body = std::make_unique<btRigidBody>(info);

		// Register body with dynamics world
		dk::engine::physics.register_rigid_body(controller->m_rigid_body.get());
		controller->m_rigid_body->setSleepingThresholds(0.025f, 0.01f);
    }

    void CharacterControllerSystem::on_late_tick(float delta_time)
    {
        for(Handle<CharacterController> controller : *this)
        {

        }
    }

    void CharacterControllerSystem::on_end()
    {
        Handle<CharacterController> controller = get_component();
    }
}