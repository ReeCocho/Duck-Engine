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

		/**
		 * @brief Move the player.
		 * @param Position delta.
		 */
		void move(glm::vec3 del);

		/**
		 * @brief Set radius.
		 * @param New radius.
		 * @return New radius. 
		 */
		float set_radius(float r);

		/**
		 * @brief Set height.
		 * @param New height.
		 * @return New height.
		 */
		float set_height(float h);

		/**
		 * @brief Get radius.
		 * @return Radius. 
		 */
		float get_radius() const
		{
			return static_cast<float>(m_shape->getRadius());
		}

		/**
		 * @brief Get height.
		 * @return Height. 
		 */
		float get_height() const
		{
			return static_cast<float>(m_shape->getHalfHeight() * 2.0f);
		}

    private:

        /** Entities transform. */
		Handle<Transform> m_transform = {};

		/** Collision shape. */
		std::unique_ptr<btCapsuleShape> m_shape;

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