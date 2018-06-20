#pragma once

/**
 * @file rigidbody.hpp
 * @brief Component that performs physics and collisions.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <physics\physics.hpp>
#include <ecs\scene.hpp>
#include "transform.hpp"

namespace dk
{
	/**
	 * @brief Type of collision shape.
	 */
	enum class ShapeType
	{
		None = 0,
		Box = 1,
		Sphere = 2,
		Capsule = 3
	};

    /**
	 * @brief Component that performs physics and collisions.
	 */
	class RigidBody : public Component<RigidBody>
	{
		friend class RigidBodySystem;

	public:

		DK_COMPONENT_BODY(RigidBody)

        /**
         * @brief Get shape type.
         * @return Shape type. 
         */
        ShapeType get_shape_type() const
        {
            return m_data.shape_type;
        }

        /**
		 * @brief Remove shape.
		 */
		void set_shape_none();

		/**
		 * @brief Set box shape.
		 * @param Dimensions.
		 */
		void set_box_shape(glm::vec3 dimensions);

		/**
		 * @brief Set sphere shape.
		 * @param Radius.
		 */
		void set_sphere_shape(float radius);

		/**
		 * @brief Set capsule shape.
		 * @param Height.
		 * @param Radius.
		 */
		void set_capsule_shape(float height, float radius);

        /**
		 * @brief Get mass.
		 * @return Mass.
		 */
		float get_mass() const
		{
			return m_data.mass;
		}

		/**
		 * @brief Get linear velocity.
		 * @return Linear velocity.
		 */
		glm::vec3 get_linear_Velocity() const
		{
			auto vel = m_rigid_body->getLinearVelocity();
			return glm::vec3(vel.x(), vel.y(), vel.z());
		}

		/**
		 * @brief Get angular velocity.
		 * @return Angular velocity.
		 */
		glm::vec3 get_angular_velocity() const
		{
			auto vel = m_rigid_body->getAngularVelocity();
			return glm::vec3(vel.x(), vel.y(), vel.z());
		}

		/**
		 * @brief Get friction.
		 * @return Friction.
		 */
		float get_friction() const
		{
			return m_data.friction;
		}

		/**
		 * @brief Get rolling friction.
		 * @return Rolling friction.
		 */
		float get_rolling_friction() const
		{
			return m_data.rolling_friction;
		}

		/**
		 * @brief Get spinning friction.
		 * @return Spinning friction.
		 */
		float get_spinning_friction() const
		{
			return m_data.spinning_friction;
		}

		/**
		 * @brief Get restitution.
		 * @return Restitution.
		 */
		float get_restitution() const
		{
			return m_data.restitution;
		}

        /**
		 * @brief Set mass.
		 * @param New mass.
		 * @return New mass.
		 */
		float set_mass(float mass);

		/**
		 * @brief Set the position of the rigid body.
		 * @param New position.
		 * @return New position.
		 */
		glm::vec3 set_position(glm::vec3 pos);

		/**
		 * @brief Set the local position of the rigid body.
		 * @param New position.
		 * @return New position.
		 */
		glm::vec3 set_local_position(glm::vec3 pos);

		/**
		 * @brief Set the euler angles of the rigid body.
		 * @param New euler angles.
		 * @return New euler angles.
		 */
		glm::vec3 set_euler_angles(glm::vec3 rot);

		/**
		 * @brief Set the local euler angles of the rigid body.
		 * @param New euler angles.
		 * @return New euler angles.
		 */
		glm::vec3 set_local_euler_angles(glm::vec3 rot);

		/**
		 * @brief Set the rotation of the rigid body.
		 * @param New rotation.
		 * @return New rotation.
		 */
		glm::quat set_rotation(glm::quat rot);

		/**
		 * @brief Set the local rotation of the rigid body.
		 * @param New rotation.
		 * @return New rotation.
		 */
		glm::quat set_local_rotation(glm::quat rot);

        /**
		 * @brief Set linear velocity.
		 * @param New linear velocity.
		 * @return New linear velocity.
		 */
		glm::vec3 set_linear_velocity(glm::vec3 vel);

        /**
		 * @brief Set angular velocity.
		 * @param New angular velocity.
		 * @return New angular velocity.
		 */
		glm::vec3 set_angular_velocity(glm::vec3 vel);

		/**
		 * @brief Set friction.
		 * @param New friction.
		 * @return New friction.
		 */
		float set_friction(float frict);

		/**
		 * @brief Set rolling friction.
		 * @param New rolling friction.
		 * @return New rolling friction.
		 */
		float set_rolling_friction(float frict);

		/**
		 * @brief Set spinning friction.
		 * @param New spinning friction.
		 * @return New spinning friction.
		 */
		float set_spinning_friction(float frict);

		/**
		 * @brief Set rolling, spinning, and sliding friction.
		 * @param New frictions.
		 * @return New frictions.
		 */
		float set_all_frictions(float frict);

		/**
		 * @brief Set restitution.
		 * @param New restitution.
		 * @return New restitution.
		 */
		float set_restitution(float rest);

		/**
		 * @brief Set if the rigid body is static.
		 * @param If the rigid body is static.
		 * @return If the rigid body is static.
		 * @note This will also set the mass to 0.
		 */
		bool set_static(bool s);

    private:

        /*
         * @brief Calculate inertia.
         */
        void calculate_inertia();

		/**
		 * Shape data to serialize.
		 */
		struct
		{
			/** Shape type. */
			ShapeType shape_type = ShapeType::None;

			/** Mass. */
			float mass = 1.0f;

			/** Is the object static. */
			bool is_static = false;

			/** Friction. */
			float friction = 0.25f;

			/** Rolling friction. */
			float rolling_friction = 0.25f;

			/** Spinning friction. */
			float spinning_friction = 0.25f;

			/** Restitution. */
			float restitution = 0.25f;

			/**
			 * Shape union.
			 */
			union
			{
				struct
				{
					float radius;
				} sphere;

				glm::vec3 box = {};

				struct
				{
					float radius;
					float height;
				} capsule;
			} shape = {};

		} m_data;

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
	 * @brief Implementation of the rigid body.
	 */
	class RigidBodySystem : public System<RigidBody>
	{
	public:

		DK_SYSTEM_BODY(RigidBodySystem, RigidBody, true)

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

		/**
		 * Serialize the active component.
		 * @param Component archiver.
		 */
		void serialize(ReflectionContext& r) override;

		/**
		 * Inspect the active component.
		 * @param Reflection context.
		 */
		void inspect(ReflectionContext& r) override;
	};
}