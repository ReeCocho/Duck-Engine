/* Includes. */
#include <ecs\transform.hpp>
#include <engine.hpp>
#include <config.hpp>
#include "rigidbody.hpp"

namespace dk
{
    void RigidBody::calculate_inertia()
    {
        btVector3 inertia = { 0, 0, 0 };
		if (get_mass() != 0)
			m_shape->calculateLocalInertia(get_mass(), inertia);
		m_rigid_body->setMassProps(get_mass(), inertia);
    }

    void RigidBody::set_shape_none()
    {
		m_shape_type = ShapeType::None;

        // Make empty shape
		m_shape = std::make_unique<btEmptyShape>();
		m_rigid_body->setCollisionShape(m_shape.get());

        calculate_inertia();
    }

    void RigidBody::set_box_shape(glm::vec3 dimensions)
    {
		m_shape_type = ShapeType::Box;

        // Make box shape
		m_shape = std::make_unique<btBoxShape>(btVector3(dimensions.x, dimensions.y, dimensions.z) / 2.0f);
		m_rigid_body->setCollisionShape(m_shape.get());

		calculate_inertia();
    }

    void RigidBody::set_sphere_shape(float radius)
    {
		m_shape_type = ShapeType::Sphere;

        // Make sphere shape
		m_shape = std::make_unique<btSphereShape>(radius);
		m_rigid_body->setCollisionShape(m_shape.get());

		calculate_inertia();
    }

    void RigidBody::set_capsule_shape(float height, float radius)
    {
        m_shape_type = ShapeType::Capsule;

        // Make capsule shape
		m_shape = std::make_unique<btCapsuleShape>(radius, height);
		m_rigid_body->setCollisionShape(m_shape.get());

		calculate_inertia();
    }

    float RigidBody::set_mass(float mass)
    {
        btVector3 inertia(0, 0, 0);
        m_rigid_body->activate(true);

        if (mass != 0)
            m_shape->calculateLocalInertia(mass, inertia);

        m_rigid_body->setMassProps(mass, inertia);
        return mass;
    }

    glm::vec3 RigidBody::set_linear_velocity(glm::vec3 vel)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
        return vel;
    }

    glm::vec3 RigidBody::set_angular_velocity(glm::vec3 vel)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setAngularVelocity(btVector3(vel.x, vel.y, vel.z));
        return vel;
    }

    float RigidBody::set_friction(float frict)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setFriction(frict);
        return frict;
    }

    float RigidBody::set_rolling_friction(float frict)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setRollingFriction(frict);
        return frict;
    }

    float RigidBody::set_spinning_friction(float frict)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setSpinningFriction(frict);
        return frict;
    }

    float RigidBody::set_all_frictions(float frict)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setFriction(frict);
        m_rigid_body->setSpinningFriction(frict);
        m_rigid_body->setRollingFriction(frict);
        return frict;
    }

    float RigidBody::set_restitution(float rest)
    {
        m_rigid_body->activate(true);
        m_rigid_body->setRestitution(rest);
        return rest;
    }

    bool RigidBody::set_static(bool s)
    {
        if(s) set_mass(0);

        auto flags = !(m_rigid_body->getCollisionFlags() ^ btCollisionObject::CF_STATIC_OBJECT);
        m_rigid_body->setCollisionFlags(flags | (s ? btCollisionObject::CF_STATIC_OBJECT : 0 ));

        return s;
    }



    void RigidBodySystem::on_begin()
    {
        Handle<RigidBody> rigid_body = get_component();

        // Get transform component
		rigid_body->m_transform = rigid_body->get_entity().get_component<Transform>();
		auto pos = rigid_body->m_transform->get_position();
		auto rot = rigid_body->m_transform->get_rotation();

		// Set physics transform
		btTransform transform = {};
		transform.setIdentity();
		transform.setOrigin({ pos.x, pos.y, pos.z });
		transform.setRotation({ rot.x, rot.y, rot.z, rot.w });

		// Create motion state
		rigid_body->m_motion_state = std::make_unique<btDefaultMotionState>(transform);

		// Create shape
		rigid_body->m_shape = std::make_unique<btEmptyShape>();

		btRigidBody::btRigidBodyConstructionInfo info
		(
			1.0f,
			rigid_body->m_motion_state.get(),
			rigid_body->m_shape.get()
		);

		// Create rigid body
		rigid_body->m_rigid_body = std::make_unique<btRigidBody>(info);

		// Register body with dynamics world
		dk::engine::physics.register_rigid_body(rigid_body->m_rigid_body.get());
		rigid_body->m_rigid_body->setSleepingThresholds(0.025f, 0.01f);
    }

    void RigidBodySystem::on_late_tick(float delta_time)
    {
        for(Handle<RigidBody> rigid_body : *this)
        {
			// Get current transform
			auto cur_pos = rigid_body->m_transform->get_position();
			auto cur_rot = rigid_body->m_transform->get_rotation();

			// Get physics transform
			btTransform t = {};
			rigid_body->m_motion_state->getWorldTransform(t);
			auto pos = t.getOrigin();
			auto rot = t.getRotation();

			// Interpolate transform
			cur_pos = glm::mix(cur_pos, { pos.x(), pos.y(), pos.z() }, delta_time * DK_PHYSICS_POSITION_INTERPOLATION_RATE);
			cur_rot = glm::slerp(cur_rot, { rot.w(), rot.x(), rot.y(), rot.z() }, delta_time * DK_PHYSICS_ROTATION_INTERPOLATION_RATE);

			// Set transform
			rigid_body->m_transform->set_position(cur_pos);
			rigid_body->m_transform->set_rotation(cur_rot);
        }
    }

    void RigidBodySystem::on_end()
    {
        Handle<RigidBody> rigid_body = get_component();
        dk::engine::physics.unregister_rigid_body(rigid_body->m_rigid_body.get());
		rigid_body->m_motion_state.reset();
		rigid_body->m_shape.reset();
		rigid_body->m_rigid_body.reset();
    }
}