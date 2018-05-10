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
		 * @brief Set sliding angle.
		 * @param New sliding angle.
		 * @return New sliding angle.
		 */
		float set_sliding_angle(float sa);

		/**
		 * @brief Get ground snap.
		 * @param New ground snap.
		 * @return New ground snap.
		 */
		bool set_ground_snap(bool s);

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

		/**
		 * @brief Get sliding angle.
		 * @return Sliding angle.
		 */
		float get_sliding_angle() const
		{
			return m_sliding_angle;
		}

		/**
		 * @brief Get ground snap.
		 * @return Ground snap.
		 */
		bool get_ground_snap() const
		{
			return m_ground_snap;
		}

		/**
		 * @brief Get if the character controller is grounded.
		 * @return If the controller is grounded.
		 */
		bool is_grounded() const
		{
			return m_grounded;
		}

    private:

        /** Entities transform. */
		Handle<Transform> m_transform = {};

		/** Collision shape. */
		std::unique_ptr<btCapsuleShape> m_shape;

		/** Rigid body. */
		std::unique_ptr<btRigidBody> m_rigid_body;

		/** Ghost for collisions. */
		std::unique_ptr<btPairCachingGhostObject> m_ghost;

		/** Is the controller grounded? */
		bool m_grounded = false;

		/** Angle at which the controller will slide. */
		float m_sliding_angle = 60.0f;

		/** Should the controller snap to the ground? */
		bool m_ground_snap = true;

		/** Keeps track of contack manifolds. */
		btManifoldArray	m_manifold_array;
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