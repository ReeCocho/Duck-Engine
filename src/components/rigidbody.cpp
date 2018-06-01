/* Includes. */
#include "transform.hpp"
#include <common.hpp>
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

    glm::vec3 RigidBody::set_position(glm::vec3 pos)
    {
        auto new_pos = m_transform->set_position(pos);
        auto transform = m_rigid_body->getWorldTransform();
        transform.setOrigin(btVector3(new_pos.x, new_pos.y, new_pos.z));
        m_rigid_body->setWorldTransform(transform);
        return new_pos;
    }

    glm::vec3 RigidBody::set_local_position(glm::vec3 pos)
    {
        auto new_pos = m_transform->set_local_position(pos);
        auto global_pos = m_transform->get_position();
        auto transform = m_rigid_body->getWorldTransform();
        transform.setOrigin(btVector3(global_pos.x, global_pos.y, global_pos.z));
        m_rigid_body->setWorldTransform(transform);
        return new_pos;
    }

    glm::vec3 RigidBody::set_euler_angles(glm::vec3 rot)
    {
        auto new_rot = m_transform->set_euler_angles(rot);
        auto transform = m_rigid_body->getWorldTransform();
        auto quat = btQuaternion();
        quat.setEuler(glm::radians(new_rot.y), glm::radians(new_rot.x), glm::radians(new_rot.z));
        transform.setRotation(quat);
        m_rigid_body->setWorldTransform(transform);
        return new_rot;
    }

    glm::vec3 RigidBody::set_local_euler_angles(glm::vec3 rot)
    {
        auto new_rot = m_transform->set_local_euler_angles(rot);
        auto global_rot = m_transform->get_euler_angles();
        auto transform = m_rigid_body->getWorldTransform();
        auto quat = btQuaternion();
        quat.setEuler(glm::radians(global_rot.y), glm::radians(global_rot.x), glm::radians(global_rot.z));
        transform.setRotation(quat);
        m_rigid_body->setWorldTransform(transform);
        return new_rot;
    }

    glm::quat RigidBody::set_rotation(glm::quat rot)
    {
        auto new_rot = m_transform->set_rotation(rot);
        auto transform = m_rigid_body->getWorldTransform();
        transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
        m_rigid_body->setWorldTransform(transform);
        return new_rot;
    }

    glm::quat RigidBody::set_local_rotation(glm::quat rot)
    {
        auto new_rot = m_transform->set_local_rotation(rot);
        auto global_rot = m_transform->get_rotation();
        auto transform = m_rigid_body->getWorldTransform();
        transform.setRotation(btQuaternion(global_rot.x, global_rot.y, global_rot.z, global_rot.w));
        m_rigid_body->setWorldTransform(transform);
        return new_rot;
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
#if !DK_EDITOR
		dk::engine::physics.register_rigid_body(rigid_body->m_rigid_body.get());
#endif
		rigid_body->m_rigid_body->setSleepingThresholds(DK_PHYSICS_LINEAR_SLEEP_THRESHOLD, DK_PHYSICS_ANGULAR_SLEEP_THRESHOLD);
    }

    void RigidBodySystem::on_late_tick(float delta_time)
    {
#if !DK_EDITOR
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
#endif
    }

    void RigidBodySystem::on_end()
    {
        Handle<RigidBody> rigid_body = get_component();
#if !DK_EDITOR
        dk::engine::physics.unregister_rigid_body(rigid_body->m_rigid_body.get());
#endif
		rigid_body->m_motion_state.reset();
		rigid_body->m_shape.reset();
		rigid_body->m_rigid_body.reset();
    }

	void RigidBodySystem::serialize(ReflectionContext& archive)
	{
		ComponentArchive& a = static_cast<ComponentArchive&>(archive);
		Handle<RigidBody> body = get_component();
		a.set_name("Rigid Body");
	}
}