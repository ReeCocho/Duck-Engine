/* Includes. */
#include <algorithm>
#include "physics.hpp"

namespace dk
{
    Physics::Physics(glm::vec3 gravity)
    {
        // Create collision configuration
		m_collision_config = std::make_unique<btDefaultCollisionConfiguration>();

		// Create dispatcher
		m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collision_config.get());

		// Create broadphase
		m_broadphase = std::make_unique<btDbvtBroadphase>();

		// Create constraint solver
		m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

		// Create dynamics world
		m_dynamics_world = std::make_unique<btDiscreteDynamicsWorld>
		(
			m_dispatcher.get(),
			m_broadphase.get(),
			m_solver.get(),
			m_collision_config.get()
		);

        // Callbacks for ghost objects
		m_dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

        // Set solver iteration count
		auto& solver = m_dynamics_world->getSolverInfo();
		solver.m_numIterations = 50;

		// Set gravity vector
		m_dynamics_world->setGravity({ gravity.x, gravity.y, gravity.z });

		// Enable static on static collision reporting (Gets rid of a warning)
		m_dispatcher->setDispatcherFlags(m_dispatcher->getDispatcherFlags() | btCollisionDispatcher::CD_STATIC_STATIC_REPORTED);
    }

    void Physics::shutdown()
    {
        m_dynamics_world.reset();
		m_solver.reset();
		m_broadphase.reset();
		m_dispatcher.reset();
		m_collision_config.reset();
    }

    void Physics::step(float delta_time)
    {
        // Do a step
		m_dynamics_world->stepSimulation(delta_time, 50, 1 / 144.0f);
    }

    void Physics::register_rigid_body(btRigidBody* body)
    {
        m_dynamics_world->addRigidBody(body);
        m_bodies.push_back(body);
    }

    void Physics::register_collision_object(btCollisionObject* obj)
    {
        m_dynamics_world->addCollisionObject(obj);
    }

    void Physics::unregister_rigid_body(btRigidBody* body)
    {
        m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), body), m_bodies.end());
        m_dynamics_world->removeRigidBody(body);
    }

    void Physics::unregister_collision_object(btCollisionObject* obj)
    {
        m_dynamics_world->removeCollisionObject(obj);
    }
}