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
		btTransform t = m_ghost->getWorldTransform();
		t.setOrigin(t.getOrigin() + btVector3(del.x, del.y, del.z));
		m_rigid_body->setWorldTransform(t);
		m_ghost->setWorldTransform(t);
		m_rigid_body->activate(true);
	}

	float CharacterController::set_radius(float r)
	{
		const float h = get_height();
		m_shape = std::make_unique<btCapsuleShape>(r, h);
		m_rigid_body->setCollisionShape(m_shape.get());
		m_ghost->setCollisionShape(m_shape.get());
		return r;
	}

	float CharacterController::set_height(float h)
	{
		const float r = get_radius();
		m_shape = std::make_unique<btCapsuleShape>(r, h);
		m_rigid_body->setCollisionShape(m_shape.get());
		m_ghost->setCollisionShape(m_shape.get());
		return h;
	}

	float CharacterController::set_sliding_angle(float sa)
	{
		m_sliding_angle = sa;
		return m_sliding_angle;
	}

	bool CharacterController::set_ground_snap(bool s)
	{
		m_ground_snap = s;
		return m_ground_snap;
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

		// Create shape
		controller->m_shape = std::make_unique<btCapsuleShape>(0.25f, 1.5f);

		btRigidBody::btRigidBodyConstructionInfo info
		(
			1.0f,
			nullptr,
			controller->m_shape.get()
		);
		info.m_startWorldTransform = transform;

		// Create rigid body
		controller->m_rigid_body = std::make_unique<btRigidBody>(info);
		controller->m_rigid_body->setGravity(btVector3(0, 0, 0));
		controller->m_rigid_body->setAngularFactor(0);
		controller->m_rigid_body->setLinearFactor(btVector3(0, 0, 0));

		// Create ghost
		controller->m_ghost = std::make_unique<btGhostObject>();
		controller->m_ghost->setCollisionShape(controller->m_shape.get());
		controller->m_ghost->setWorldTransform(transform);

		// Register bodies with dynamics world
		dk::engine::physics.register_rigid_body(controller->m_rigid_body.get());
		dk::engine::physics.register_collision_object(controller->m_ghost.get());
		controller->m_rigid_body->setSleepingThresholds(DK_PHYSICS_LINEAR_SLEEP_THRESHOLD, DK_PHYSICS_ANGULAR_SLEEP_THRESHOLD);
    }

    void CharacterControllerSystem::on_late_tick(float delta_time)
    {
        for(Handle<CharacterController> controller : *this)
        {
			// Get current transform
			auto cur_pos = controller->m_transform->get_position();

			// Get physics transform
			btTransform t = controller->m_ghost->getWorldTransform();
			btVector3 pos = t.getOrigin();

			// Move away from every collision and detect grounding
			const auto& col_dat = dk::engine::physics.get_collision_data(controller->m_rigid_body.get());
			float cos_sa = glm::cos(glm::radians(controller->m_sliding_angle));
			glm::vec3 pos_diff = {};
			glm::vec3 bottom_pos = { pos.x(), pos.y() - (controller->get_height() / 2.0f), pos.z() };
			controller->m_grounded = false;
			
			for (const auto dat : col_dat)
				if 
				(
					dat.touched != nullptr && 
					dat.touching != nullptr && 
					dat.touched != controller->m_ghost.get() && 
					dat.touching != controller->m_ghost.get()
				)
				{
					glm::vec3 del = dat.normal * -dat.penetration * DK_PHYSICS_STEP_RATE;
					pos_diff.x += del.x;
					pos_diff.z += del.z;
				}

			// Ground snap
			if (controller->m_ground_snap)
			{
				RaycastHitData data = dk::engine::physics.raycast
				(
					bottom_pos,
					glm::vec3(0, -1, 0),
					controller->get_radius() + DK_CHARACTER_CONTROLLER_GRND_EPS
				);

				if (data.hit)
				{
					controller->m_grounded = true;
					pos.setY(data.point.y + controller->get_radius() + (controller->get_height() / 2.0f));
				}
			}

			// // Change position
			pos += btVector3(pos_diff.x, pos_diff.y, pos_diff.z);
			t.setOrigin(pos);
			controller->m_rigid_body->setWorldTransform(t);
			controller->m_ghost->setWorldTransform(t);

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
		dk::engine::physics.unregister_collision_object(controller->m_ghost.get());
		controller->m_shape.reset();
		controller->m_rigid_body.reset();
    }
}