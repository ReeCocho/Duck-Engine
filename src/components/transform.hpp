#pragma once

/**
 * @file transform.hpp
 * @brief Transform header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <engine/archive.hpp>
#include <ecs/scene.hpp>

namespace dk
{
	/**
	 * @class Transform
	 * @brief Allows an entiy to be represented in world space.
	 */
	class Transform : public Component<Transform>
	{
		friend class TransformSystem;

	public:

		DK_COMPONENT_BODY(Transform)

		/**
		 * @brief Get the transforms position.
		 * @return Position.
		 */
		glm::vec3 get_position() const
		{
			return m_position;
		}

		/**
		 * @brief Get the transform local position.
		 * @return Local position.
		 */
		glm::vec3 get_local_position() const
		{
			return m_local_position;
		}

		/**
		 * @brief Get the transforms rotation.
		 * @return Rotation.
		 */
		glm::quat get_rotation() const
		{
			return m_rotation;
		}

		/**
		 * @brief Get the transforms local rotation.
		 * @return Local rotation.
		 */
		glm::quat get_local_rotation() const
		{
			return m_local_rotation;
		}

		/**
		 * @brief Get the transforms euler angles.
		 * @return Euler angles.
		 */
		glm::vec3 get_euler_angles() const
		{
			return m_euler_angles;
		}

		/**
		 * @brief Get the transforms local euler angles.
		 * @return Local euler angles.
		 */
		glm::vec3 get_local_euler_angles() const
		{
			return m_local_euler_angles;
		}

		/**
		 * @brief Get the transforms local scale.
		 * @return Local scale.
		 */
		glm::vec3 get_local_scale() const
		{
			return m_local_scale;
		}

		/**
		 * @brief Get the transforms model matrix.
		 * @return Model matrix.
		 */
		glm::mat4 get_model_matrix() const
		{
			return m_model_matrix;
		}

		/**
		 * @brief Get a forward vector realative to the transform.
		 * @return Forward vector.
		 */
		glm::vec3 get_forward() const
		{
			auto mat = glm::mat4_cast(m_rotation);
			auto front = glm::vec3(mat[2].x, mat[2].y, mat[2].z);

			return front;
		}

		/**
		 * @brief Get a up vector realative to the transform.
		 * @return Up vector.
		 */
		glm::vec3 get_up() const
		{
			auto mat = glm::mat4_cast(m_rotation);
			auto up = glm::vec3(mat[1].x, mat[1].y, mat[1].z);

			return up;
		}

		/**
		 * @brief Get a right vector realative to the transform.
		 * @return Right vector.
		 */
		glm::vec3 get_right() const
		{
			auto mat = glm::mat4_cast(m_rotation);
			auto right = glm::vec3(mat[0].x, mat[0].y, mat[0].z);

			return right;
		}

		/**
		 * @brief Get the transforms parent.
		 * @return Transforms parent.
		 */
		Handle<Transform> get_parent() const
		{
			return m_parent;
		}

		/**
		 * @brief Get the transforms Nth child.
		 * @return Nth child.
		 * @note Returns nullptr if n is out of bounds.
		 */
		Handle<Transform> get_child(size_t n) const
		{
			dk_assert(n < m_children.size());
			return m_children[n];
		}

		/**
		 * @brief Get the number of children the transform contains.
		 * @return Number of children.
		 */
		size_t child_count() const
		{
			return m_children.size();
		}



		/**
		 * @brief Set the transforms position.
		 * @param New position.
		 * @return New position.
		 */
		glm::vec3 set_position(glm::vec3 value);

		/**
		 * @brief Set the transforms local position.
		 * @param New local position.
		 * @return New local position.
		 */
		glm::vec3 set_local_position(glm::vec3 value);

		/**
		 * @brief Set the transforms rotation.
		 * @param New rotation.
		 * @return New rotation.
		 */
		glm::quat set_rotation(glm::quat value);

		/**
		 * @brief Set the transforms local rotation.
		 * @param New local rotation.
		 * @return New local rotation.
		 */
		glm::quat set_local_rotation(glm::quat value);

		/**
		 * @brief Set the transforms euler angles.
		 * @param New euler angles.
		 * @return New euler angles.
		 */
		glm::vec3 set_euler_angles(glm::vec3 value);

		/**
		 * @brief Set the transforms local euler angles.
		 * @param New local euler angles.
		 * @return New local euler angles.
		 */
		glm::vec3 set_local_euler_angles(glm::vec3 value);

		/**
		 * @brief Set the transforms local scale.
		 * @param New local scale.
		 * @return New local scale.
		 */
		glm::vec3 set_local_scale(glm::vec3 value);

		/**
		 * @brief Set the transforms parent.
		 * @param New parent transform.
		 * @return New parent transform.
		 */
		Handle<Transform> set_parent(Handle<Transform> parent);


		/**
		 * @brief Add the position dispalcement to the old position.
		 * @param Position displacement.
		 * @return New position.
		 */
		glm::vec3 mod_position(glm::vec3 value)
		{
			return set_position(m_position + value);
		}

		/**
		 * @brief Add the local position dispalcement to the old local position.
		 * @param Local position displacement.
		 * @return New local position.
		 */
		glm::vec3 mod_local_position(glm::vec3 value)
		{
			return set_local_position(m_local_position + value);
		}

		/**
		 * @brief Multiply the rotation dispalcement by the old rotation.
		 * @param Rotation displacement.
		 * @return New rotation.
		 */
		glm::quat mod_rotation(glm::quat value)
		{
			return set_rotation(m_rotation * value);
		}

		/**
		 * @brief Multiply the local rotation dispalcement by the old local rotation.
		 * @param Local rotation displacement.
		 * @return New local rotation.
		 */
		glm::quat mod_local_rotation(glm::quat value)
		{
			return set_local_rotation(m_local_rotation * value);
		}

		/**
		 * @brief Add the euler angles dispalcement to the old euler angles.
		 * @param Euler angles displacement.
		 * @return New euler angles.
		 */
		glm::vec3 mod_euler_angles(glm::vec3 value)
		{
			return set_euler_angles(m_euler_angles + value);
		}

		/**
		 * @brief Add the local euler angles dispalcement to the old local euler angles.
		 * @param Local euler angles displacement.
		 * @return New local euler angles.
		 */
		glm::vec3 mod_local_euler_angles(glm::vec3 value)
		{
			return set_local_euler_angles(m_local_euler_angles + value);
		}

		/**
		 * @brief Add the local scale dispalcement to the old local scale.
		 * @param Local scale displacement.
		 * @return New local scale.
		 */
		glm::vec3 mod_local_scale(glm::vec3 value)
		{
			return set_local_scale(m_local_scale + value);
		}

		/**
		 * @brief Add a new child to the transform.
		 * @param Transform component of the new child.
		 */
		Handle<Transform> add_child(Handle<Transform> obj)
		{
			obj->set_parent(get_handle());
			return obj;
		}

	private:

		/**
		 * @brief Generate a new model matrix.
		 */
		void generate_model_matrix();

		/**
		 * @brief Update the transforms children.
		 */
		void update_children();



		/** Postion. */
		glm::vec3 m_position = {};

		/** Local position. */
		glm::vec3 m_local_position = {};

		/** Euler angles. */
		glm::vec3 m_euler_angles = {};

		/** Local euler angles. */
		glm::vec3 m_local_euler_angles = {};

		/** Local scale. */
		glm::vec3 m_local_scale = { 1, 1, 1 };

		/** Rotation. */
		glm::quat m_rotation = {};

		/** Local rotation. */
		glm::quat m_local_rotation = {};

		/** Model matrix. */
		glm::mat4 m_model_matrix = {};

		/** Model matrix without scale applied (Used for hierarchy.) */
		glm::mat4 m_unscaled_model_matrix = {};

		/** Children. */
		std::vector<Handle<Transform>> m_children = {};

		/** Parent. */
		Handle<Transform> m_parent = Handle<Transform>();
	};



	/**
	 * @class TransformSystem
	 * @brief Tranform system.
	 */
	class TransformSystem : public System<Transform>
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the system is in.
		 */
		TransformSystem(Scene* scene) : System<Transform>(scene, true, 32) {}

		/**
		 * @brief Destructor.
		 */
		~TransformSystem() = default;

		/**
		 * Called when a new entity is created.
		 * @param Entity created.
		 */
		void on_new_entity(Entity entity) override;

		/**
		 * @brief Called when a component is added to the system.
		 * @param Component to act upon.
		 */
		void on_begin() override;

		/**
		 * @brief Called when a component is removed from the system.
		 */
		void on_end() override;

		/**
		 * Serialize a component.
		 * @param Archiver.
		 */
		void serialize(ReflectionContext& archive) override;
	};
}