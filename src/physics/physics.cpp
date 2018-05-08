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
        // Clear old collision data
        for(auto& col_data : m_collision_data)
            col_data.second.clear();

        // Do a step
		m_dynamics_world->stepSimulation(delta_time, 50, 1 / 144.0f);

        // Get number of manifolds and loop over them
		int num_manifolds = m_dynamics_world->getDispatcher()->getNumManifolds();
		for (int i = 0; i < num_manifolds; ++i)
		{
			// Get objects colliding
			btPersistentManifold* contact_manifold = m_dynamics_world->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contact_manifold->getBody0();
			const btCollisionObject* obB = contact_manifold->getBody1();

			// Get number of contacts and loop over them
			int num_contacts = contact_manifold->getNumContacts();
			for (int j = 0; j < num_contacts; ++j)
			{
				btManifoldPoint& pt = contact_manifold->getContactPoint(j);
				if (pt.getDistance() <= 0.0f)
				{
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& ptB = pt.getPositionWorldOnB();
					const btVector3& normalOnB = pt.m_normalWorldOnB;

					btRigidBody* rbA = nullptr;
					btRigidBody* rbB = nullptr;

					// Find bodies
					for (auto rigid_body : m_bodies)
					{
						if (rbA && rbB)
							break;

						if (obA == rigid_body)
							rbA = rigid_body;

						if (obB == rigid_body)
							rbB = rigid_body;
					}

					PhysicsCollisionData data = {};
					data.point = { ptA.x(), ptA.y(), ptA.z() };
					data.normal = { normalOnB.x(), normalOnB.y(), normalOnB.z() };
					data.touched = rbB;
					data.touching = rbA;
					data.penetration = pt.getDistance();

					// Add collision data
					m_collision_data[rbA].push_back(data);
				}
			}
		}
    }

    void Physics::register_rigid_body(btRigidBody* body)
    {
        m_dynamics_world->addRigidBody(body);
        m_collision_data[body] = {};
        m_bodies.push_back(body);
    }

    void Physics::register_collision_object(btCollisionObject* obj)
    {
        m_dynamics_world->addCollisionObject(obj);
    }

    void Physics::unregister_rigid_body(btRigidBody* body)
    {
        m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), body), m_bodies.end());
        m_collision_data.erase(body);
        m_dynamics_world->removeRigidBody(body);
    }

    void Physics::unregister_collision_object(btCollisionObject* obj)
    {
        m_dynamics_world->removeCollisionObject(obj);
    }
}