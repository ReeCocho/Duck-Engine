#pragma once

/**
 * @file physics.hpp
 * @brief Physics system header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <memory>
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

namespace dk
{
    /**
     * @brief Physics engine powered by Bullet Physics.
     */
    class Physics
    {
    public:

        /**
         * @brief Default constructor.
         */
        Physics() = default;

        /**
         * @brief Constructor.
         * @param Gravity vector.
         */
        Physics(glm::vec3 gravity);

        /**
         * @brief Default destructor.
         */
        ~Physics() = default;

        /**
         * @brief Shutdown the physics engine.
         */
        void shutdown();

        /**
         * @brief Perform a step in the physics simulation.
         * @param Time since last tick.
         */
        void step(float delta_time);

        /**
         * @brief Register a rigid body.
         * @param Rigid body to register.
         */
        void register_rigid_body(btRigidBody* body);

        /**
         * @brief Register a collision object.
         * @param Collision object to register. 
         */
        void register_collision_object(btCollisionObject* obj);

        /**
         * @brief Unregister a rigid body.
         * @param Rigid body to unregister.
         */
        void unregister_rigid_body(btRigidBody* body);

        /**
         * @brief Unregister a collision object.
         * @param Collision object to unregister. 
         */
        void unregister_collision_object(btCollisionObject* obj);

    private:  

        /** Collision configuration. */
		std::unique_ptr<btDefaultCollisionConfiguration> m_collision_config;

		/** Collision dispatcher. */
		std::unique_ptr<btCollisionDispatcher> m_dispatcher;

		/** Broadphase. */
		std::unique_ptr<btBroadphaseInterface> m_broadphase;

		/** Constraint solver. */
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;

		/** Dynamics world. */
		std::unique_ptr<btDiscreteDynamicsWorld> m_dynamics_world;

        /** List of rigid bodies. */
        std::vector<btRigidBody*> m_bodies = {};
    };
}