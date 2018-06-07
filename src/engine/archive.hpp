#pragma once

/**
 * @file archive.hpp
 * @brief Archive header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <ecs\scene.hpp>
#include <vector>
#include <functional>
#include <utilities\reflection.hpp>
#include <utilities\archive.hpp>
#include "resource_manager.hpp"

namespace dk
{
	/**
	 * Component archiving object.
	 */
	class ComponentArchive : public ReflectionContext
	{
	public:

		/**
		 * Constructor.
		 * @param Scene to archive with.
		 * @param Resource manager to use for certain handles.
		 * @param Archive to read from/write to.
		 * @note The archive must not be mullptr.
		 */
		ComponentArchive(Scene* scene, ResourceManager* resource_manager, Archive* archive);

		/**
		 * Destructor.
		 */
		~ComponentArchive() = default;

		/**
		 * Detect if the archive is in writing mode or not.
		 * @return If the archive is in writing mode or not.
		 */
		bool is_writing() const
		{
			return m_archive->is_writing();
		}

		/**
		 * Either register a new field or read a field from the archive.
		 * @tparam Field type.
		 * @param Pointer to field destination/source.
		 */
		template<typename T>
		void field(T& data)
		{
			if (m_archive->is_writing())
				// Write field to archive
				m_archive->write<T>(data);
			else
				// Read from the archive
				data = m_archive->read<T>();
		}

		/**
		 * Either register a new field or read a field from the archive.
		 * @tparam Component handle field type.
		 * @param Pointer to component handle field destination/source.
		 */
		template<typename T>
		void field(Handle<T>& data)
		{
			static_assert(std::is_convertible<T, Component<T>>::value, "T must derive from Component<T>.");

			if (m_archive->is_writing())
			{
				// Null handle
				if (data == Handle<T>())
				{
					m_archive->write<std::string>("");
					m_archive->write<ResourceID>(0);
				}
				else
				{
					// Write system name
					std::string system_name = m_scene->get_system_by_base(TypeID<T>::id())->get_name();
					m_archive->write<std::string>(system_name);

					// Write component resource ID
					m_archive->write<ResourceID>(data.id);
				}
			}
			else
			{
				// Get system by name
				std::string system_name = m_archive->read<std::string>();

				// Set ID (even if the system doesn't exist
				data.id = m_archive->read<ResourceID>();

				// Null handle
				if (system_name == "")
					data.allocator = nullptr;
				else
				{
					SystemBase* system = m_scene->get_system_by_name(system_name);
					dk_assert(system);

					// Create handle
					data.allocator = static_cast<ResourceAllocator<T>*>(system->get_component_allocator());
				}
			}
		}

		/**
		 * Either register a new field or read a field from the archive.
		 * @tparam Vector type.
		 * @param Pointer to destination/source field.
		 */
		template<typename T>
		void field(std::vector<T>& data)
		{
			if (m_archive->is_writing())
			{
				// Write vector size
				m_archive->write(static_cast<uint32_t>(data.size()));

				// Write vector elements
				for (size_t i = 0; i < data.size(); ++i)
					field(data[i]);
			}
			else
			{
				// Read data size
				uint32_t len = m_archive->read<uint32_t>();
				data.resize(len);

				// Read elements
				for (uint32_t i = 0; i < len; ++i)
					field(data[i]);
			}
		}

	private:

		/** Archive we are reading/writing from/to. */
		Archive* m_archive;

		/** Scene. */
		Scene* m_scene;

		/** Resource manager. */
		ResourceManager* m_resource_manager;
	};

	// Specializations

	template<>
	void ComponentArchive::field<Mesh>(HMesh& data);

	template<>
	void ComponentArchive::field<MaterialShader>(HMaterialShader& data);

	template<>
	void ComponentArchive::field<Material>(HMaterial& data);

	template<>
	void ComponentArchive::field<SkyBox>(HSkyBox& data);

	template<>
	void ComponentArchive::field<CubeMap>(HCubeMap& data);
}