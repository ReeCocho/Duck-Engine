/** Includes. */
#include "transform.hpp"
#include <engine\common.hpp>
#include <engine\config.hpp>
#include <editor\component_inspector.hpp>
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
		dk_assert(r > 0.0f);
		const float h = get_height();
		m_shape = std::make_unique<btCapsuleShape>(r, h);
		m_rigid_body->setCollisionShape(m_shape.get());
		m_ghost->setCollisionShape(m_shape.get());
		return r;
	}

	float CharacterController::set_height(float h)
	{
		dk_assert(h >= 0.0f);
		const float r = get_radius();
		m_shape = std::make_unique<btCapsuleShape>(r, h);
		m_rigid_body->setCollisionShape(m_shape.get());
		m_ghost->setCollisionShape(m_shape.get());
		return h;
	}

	float CharacterController::set_sliding_angle(float sa)
	{
		dk_assert(sa >= 0.0f && sa < 90.0f);
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
		const auto pos = controller->m_transform->get_position();
		const auto rot = controller->m_transform->get_rotation();

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
		controller->m_rigid_body->setFriction(0.0f);
		controller->m_rigid_body->setRestitution(0.0f);

		// Create ghost
		controller->m_ghost = std::make_unique<btPairCachingGhostObject>();
		controller->m_ghost->setCollisionShape(controller->m_shape.get());
		controller->m_ghost->setWorldTransform(transform);
		controller->m_ghost->setFriction(0.0f);
		controller->m_ghost->setRestitution(0.0f);

		// Register bodies with dynamics world
#if !DK_EDITOR
		dk::engine::physics.register_rigid_body(controller->m_rigid_body.get());
		dk::engine::physics.register_collision_object(controller->m_ghost.get());
#endif
		controller->m_rigid_body->setSleepingThresholds(DK_PHYSICS_LINEAR_SLEEP_THRESHOLD, DK_PHYSICS_ANGULAR_SLEEP_THRESHOLD);
    }

    void CharacterControllerSystem::on_late_tick(float delta_time)
    {
#if !DK_EDITOR
		// Physics dynamics world
		auto& dynamics_word = dk::engine::physics.get_dynamics_world();

        for(Handle<CharacterController> controller : *this)
        {
			// Update controller in the physics world
			btVector3 minAabb, maxAabb;
			controller->m_shape->getAabb(controller->m_ghost->getWorldTransform(), minAabb, maxAabb);
			dynamics_word.getBroadphase()->setAabb
			(	
				controller->m_ghost->getBroadphaseHandle(),
				minAabb,
				maxAabb,
				dynamics_word.getDispatcher()
			);

			dynamics_word.getDispatcher()->dispatchAllCollisionPairs
			(
				controller->m_ghost->getOverlappingPairCache(), 
				dynamics_word.getDispatchInfo(),
				dynamics_word.getDispatcher()
			);

			// Get current transform position
			auto cur_pos = controller->m_transform->get_position();

			// Get physics transform
			btTransform t = controller->m_ghost->getWorldTransform();
			const btVector3 phys_pos = t.getOrigin();

			// Cosine of the sliding angle
			const float cos_sa = glm::cos(glm::radians(controller->m_sliding_angle));

			// Position to move to
			glm::vec3 new_pos = { phys_pos.x(), phys_pos.y(), phys_pos.z() };

			// Waist position of the controller
			const glm::vec3 bottom_pos = { new_pos.x, new_pos.y - (controller->get_height() / 2.0f), new_pos.z };

			// Reset grounding flag
			controller->m_grounded = false;
			
			// Loop over every ghost contact
			for (int i = 0; i < controller->m_ghost->getOverlappingPairCache()->getNumOverlappingPairs(); ++i)
			{
				// Clear old manifold array
				controller->m_manifold_array.resize(0);

				// Get objects in contact
				const btBroadphasePair* collision_pair = &controller->m_ghost->getOverlappingPairCache()->getOverlappingPairArray()[i];
				const btCollisionObject* obj0 = static_cast<btCollisionObject*>(collision_pair->m_pProxy0->m_clientObject);
				const btCollisionObject* obj1 = static_cast<btCollisionObject*>(collision_pair->m_pProxy1->m_clientObject);

				// Continue if we don't have contact or we
				// see the contact with the rigid body
				if ((obj0 && !obj0->hasContactResponse()) || 
					(obj1 && !obj1->hasContactResponse()) || 
					obj0 == controller->m_rigid_body.get() || 
					obj1 == controller->m_rigid_body.get())
					continue;

				// Get manifolds
				if (collision_pair->m_algorithm)
					collision_pair->m_algorithm->getAllContactManifolds(controller->m_manifold_array);

				// Loop over every manifold
				for (int j = 0; j < controller->m_manifold_array.size(); ++j)
				{
					// Get manifold
					const btPersistentManifold* manifold = controller->m_manifold_array[j];

					// Sign for moving away from walls
					const float direction_sign = manifold->getBody0() == controller->m_ghost.get() ? -1.0f : 1.0f;

					// Loop over every contact and move away if needed
					for (int p = 0; p < manifold->getNumContacts(); ++p)
					{
						const btManifoldPoint& pt = manifold->getContactPoint(p);

						// Is this contact ground?
						bool grounding_point = false;

						// Cosine of the angle between the contact normal and down vector.
						const float cos_cp = glm::dot(glm::vec3(0, -1, 0), { pt.m_normalWorldOnB.x(),  pt.m_normalWorldOnB.y(),  pt.m_normalWorldOnB.z() });

						// Check if we are on the ground
						if (pt.getPositionWorldOnB().y() < bottom_pos.y && cos_cp * direction_sign > cos_sa)
						{
							grounding_point = true;
							controller->m_grounded = true;
						}

						// Distance inside the contact
						const float dist = static_cast<float>(pt.getDistance());

						// Snap or push out
						if(grounding_point && controller->m_ground_snap)
							new_pos += glm::vec3(0, pt.m_normalWorldOnB.y(), 0) * (dist + 0.03f) * direction_sign;
						else if (dist < 0.0)
							new_pos += glm::vec3(pt.m_normalWorldOnB.x(), pt.m_normalWorldOnB.y(), pt.m_normalWorldOnB.z()) * dist * direction_sign;
					}
				}
			}

			// Change position
			t.setOrigin(btVector3(new_pos.x, new_pos.y, new_pos.z));
			controller->m_rigid_body->setWorldTransform(t);
			controller->m_ghost->setWorldTransform(t);

			// Interpolate transform
			cur_pos = glm::mix(cur_pos, new_pos, delta_time * DK_PHYSICS_POSITION_INTERPOLATION_RATE);

			// Set position
			controller->m_transform->set_position(cur_pos);

			// Set rotation
			controller->m_transform->set_euler_angles(glm::vec3(0, controller->m_transform->get_euler_angles().y, 0));
        }
#endif
    }

    void CharacterControllerSystem::on_end()
    {
        Handle<CharacterController> controller = get_component();
#if !DK_EDITOR
		dk::engine::physics.unregister_rigid_body(controller->m_rigid_body.get());
		dk::engine::physics.unregister_collision_object(controller->m_ghost.get());
#endif
		controller->m_shape.reset();
		controller->m_rigid_body.reset();
    }

	void CharacterControllerSystem::serialize(ReflectionContext& archive)
	{
		Handle<CharacterController> controller = get_component();

		if (auto a = dynamic_cast<ComponentArchive*>(&archive))
		{

		}
		else if (auto a = dynamic_cast<ComponentInspector*>(&archive))
		{
			a->set_name("Character Controller");
		}
	}
}