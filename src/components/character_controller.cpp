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
		btTransform t = m_rigid_body->getWorldTransform();
		t.setOrigin(t.getOrigin() + btVector3(del.x, del.y, del.z));
		m_rigid_body->setWorldTransform(t);
		m_rigid_body->activate(true);
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
		controller->m_rigid_body->setAngularFactor(0);
		controller->m_rigid_body->setLinearFactor(btVector3(0, 0, 0));

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
			btTransform t = controller->m_rigid_body->getWorldTransform();
			auto pos = t.getOrigin();

			// Get collision data
			const auto& collision_data = dk::engine::physics.get_collision_data(controller->m_rigid_body.get());

			// Move away from every collision
			glm::vec3 pos_diff = {};
			for (const auto& data : collision_data)
				if (data.penetration < 0.0f)
					pos_diff -= data.normal * data.penetration * 0.23f;

			// Change position
			pos += btVector3(pos_diff.x, pos_diff.y, pos_diff.z);
			t.setOrigin(pos);
			controller->m_rigid_body->setWorldTransform(t);

			// Check for grounding
			RaycastHitData hit_data = dk::engine::physics.raycast
			(
				{ pos.x(), pos.y(), pos.z() },
				glm::vec3(0, -1, 0),
				(controller->get_height() / 2.0f) + controller->get_radius() + DK_CHARACTER_CONTROLLER_GRND_EPS
			);

			if (hit_data.hit)
			{
				controller->m_grounded = true;
				float dist = glm::distance(hit_data.point, { pos.x(), pos.y(), pos.z() });
				dist = ((controller->get_height() / 2.0f) + controller->get_radius()) - dist;
				pos.setY(pos.y() + dist);
			}
			else
				controller->m_grounded = false;

			// Interpolate transform
			cur_pos = glm::mix(cur_pos, { pos.x(), pos.y(), pos.z() }, delta_time * DK_PHYSICS_POSITION_INTERPOLATION_RATE);

			// Set position
			controller->m_transform->set_position(cur_pos);

			// Set rotation
			controller->m_transform->set_euler_angles(glm::vec3(0, controller->m_transform->get_euler_angles().y, 0));
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