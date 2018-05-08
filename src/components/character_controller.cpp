/** Includes. */
#include <ecs\transform.hpp>
#include <engine.hpp>
#include <config.hpp>
#include "character_controller.hpp"

namespace dk
{
	void CharacterController::move(glm::vec3 del)
	{
		// Get physics transform
		btTransform t = {}; 
		m_motion_state->getWorldTransform(t);
		auto pos = t.getOrigin();
		pos += btVector3(del.x, del.y, del.z);
		m_motion_state->setWorldTransform(t);
		m_rigid_body->setWorldTransform(t);
	}

	float CharacterController::set_radius(float r)
	{
		const float h = get_height();
		m_shape = std::make_unique<btCapsuleShape>(r, h);
		m_rigid_body->setCollisionShape(m_shape.get());
		return r;
	}

	float CharacterController::set_height(float h)
	{
		const float r = get_radius();
		m_shape = std::make_unique<btCapsuleShape>(r, h);
		m_rigid_body->setCollisionShape(m_shape.get());
		return h;
	}

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
		controller->m_shape = std::make_unique<btCapsuleShape>(0.25f, 1.5f);

		btRigidBody::btRigidBodyConstructionInfo info
		(
			1.0f,
			controller->m_motion_state.get(),
			controller->m_shape.get()
		);

		// Create rigid body
		controller->m_rigid_body = std::make_unique<btRigidBody>(info);
		controller->m_rigid_body->setGravity(btVector3(0, 0, 0));
		controller->m_rigid_body->setLinearFactor(btVector3(0, 0, 0));
		controller->m_rigid_body->setAngularFactor(0);

		// Register body with dynamics world
		dk::engine::physics.register_rigid_body(controller->m_rigid_body.get());
		controller->m_rigid_body->setSleepingThresholds(DK_PHYSICS_LINEAR_SLEEP_THRESHOLD, DK_PHYSICS_ANGULAR_SLEEP_THRESHOLD);
    }

    void CharacterControllerSystem::on_late_tick(float delta_time)
    {
        for(Handle<CharacterController> controller : *this)
        {
			// Get current transform
			auto cur_pos = controller->m_transform->get_position();

			// Get physics transform
			btTransform t = {};
			controller->m_motion_state->getWorldTransform(t);
			auto pos = t.getOrigin();

			// Get collision data
			const auto& collision_data = dk::engine::physics.get_collision_data(controller->m_rigid_body.get());

			// Move away from every collision
			glm::vec3 pos_diff = {};
			for (const auto& data : collision_data)
				if (data.penetration > 0.0f)
					pos_diff -= glm::normalize(glm::vec3(pos.x(), pos.y(), pos.z()) - data.point) * data.penetration;

			// Change position
			pos += btVector3(pos_diff.x, pos_diff.y, pos_diff.z);

			// Interpolate transform
			cur_pos = glm::mix(cur_pos, { pos.x(), pos.y(), pos.z() }, delta_time * DK_PHYSICS_POSITION_INTERPOLATION_RATE);

			// Set position
			controller->m_transform->set_position(cur_pos);
        }
    }

    void CharacterControllerSystem::on_end()
    {
        Handle<CharacterController> controller = get_component();
		dk::engine::physics.unregister_rigid_body(controller->m_rigid_body.get());
		controller->m_motion_state.reset();
		controller->m_shape.reset();
		controller->m_rigid_body.reset();
    }
}