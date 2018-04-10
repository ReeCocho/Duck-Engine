#pragma once

/**
 * @file scene.hpp
 * @brief ECS scene.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <vector>
#include <memory>
#include "entity.hpp"
#include "component.hpp"
#include "system.hpp"

namespace dk
{
	/**
	 * @brief ECS scene.
	 */
	class Scene
	{
	public:

		/**
		 * @brief Constructor.
		 */
		Scene();

		/**
		 * @brief Shutdown the scene.
		 */
		void shutdown();

		/**
		 * @brief Add a system.
		 * @tparam Type of system.
		 */
		template<class T>
		void add_system()
		{
			static_assert(std::is_base_of<System<T>, T>::value, "T must derive from System<T>.");
			m_systems.push_back(std::make_unique<T>(this));
		}

		/**
		 * @brief Add a component.
		 * @tparam Type of component.
		 * @return Component handle.
		 */
		template<typename T>
		Handle<T> add_component()
		{
			for (size_t i = 0; i < m_systems.size(); ++i)
				if (m_systems[i]->get_id() == TypeID<T>.id())
				{
					System<T> system = static_cast<System<T>(m_systems[i].get());
					return system->add_component<T>();
				}

			std::runtime_error("Could not find a matching system for the component.");
		}

	private:

		/** Systems. */
		std::vector<std::unique_ptr<SystemBase>> m_systems = {};
	};
}