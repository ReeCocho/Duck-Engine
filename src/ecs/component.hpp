#pragma once

/**
 * @file component.hpp
 * @brief ECS component.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "entity.hpp"

namespace dk
{
	template<class T>
	class Component;

	/**
	 * @class TypeIDGenerator
	 * @brief A template class with a static member function to get IDs.
	 */
	template<class T>
	class TypeIDGenerator final
	{
	public:

		static const char id = 0;
	};

	/**
	 * @class TypeID
	 * @brief A template class to get ID.
	 */
	template<class T>
	class TypeID final
	{
	public:

		static_assert(std::is_base_of<Component<T>, T>::value, "T must derive from Component<T>.");

		static constexpr inline size_t id()
		{
			return reinterpret_cast<size_t>(&TypeIDGenerator<T>::id);
		}
	};



	/**
	 * @brief ECS component.
	 * @tparam Type of the component.
	 */
	template<class T>
	class Component
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Scene the component belongs to.
		 * @param Entity the component belongs to.
		 */
		Component(Scene* scene, Entity entity) : m_scene(scene), m_entity(entity), m_id(TypeID<T>::id())
		{

		}

		/**
		 * @brief Destructor.
		 */
		virtual ~Component()
		{

		}
		
		/**
		 * @brief Get the scene the component exists in.
		 * @return Scene.
		 */
		Scene& get_scene()
		{
			return *m_scene;
		}

		/**
		 * @brief Get the ID of the component.
		 * @return ID.
		 */
		size_t get_id() const
		{
			return m_id;
		}

		/**
		 * @brief Get the entity the component belongs to.
		 * @return Entity.
		 */
		Entity get_entity() const
		{
			return m_entity;
		}

	private:

		/** Scene the component is in. */
		Scene* m_scene;

		/** Entity the component belongs to. */
		Entity m_entity;

		/** ID of the component. */
		size_t m_id = 0;
	};
}