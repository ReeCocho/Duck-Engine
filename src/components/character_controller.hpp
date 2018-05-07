#pragma once

/**
 * @file character_controller.hpp
 * @brief Component that allows for character movement.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <physics\physics.hpp>
#include <ecs\scene.hpp>

namespace dk
{
    /**
     * @brief Component that allows for character movement. 
     */
    class CharacterController : public Component<CharacterController>
    {
        friend class CharacterControllerSystem;

    public:

        DK_COMPONENT_BODY(CharacterController)

    private:

        /** Entities transform. */
		Handle<Transform> m_transform = {};

		/** Collision shape. */
		std::unique_ptr<btCollisionShape> m_shape;

		/** Motion state. */
		std::unique_ptr<btMotionState> m_motion_state;

		/** Rigid body. */
		std::unique_ptr<btRigidBody> m_rigid_body;
    };



    /**
	 * @brief Implementation of the character controller.
	 */
	class CharacterControllerSystem : public System<CharacterController>
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system exists in.
		 */
		CharacterControllerSystem(Scene* scene) : System<CharacterController>(scene, 16) {}

		/**
		 * @brief Destructor.
		 */
		~CharacterControllerSystem() = default;

		/**
		 * @brief Called when a component is added to the system.
		 */
		void on_begin() override;

        /**
		 * @brief Called after on_tick().
		 * @param Time in seconds since the last frame.
		 */
		void on_late_tick(float delta_time) override;

		/**
		 * @brief Called when a component is removed from the system.
		 */
		void on_end() override;
	};
}