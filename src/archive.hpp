#pragma once

/**
 * @file archive.hpp
 * @brief Archive header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <ecs\scene.hpp>
#include <vector>
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
		 * Component archive field.
		 */
		struct Field
		{
			/** Name. */
			std::string name = "";

			/** Size of field. */
			uint32_t size = 0;

			/** Pointer to field data. */
			void* data = nullptr;

			/** Field type ID. */
			size_t type_id = 0;
		};

		/**
		 * Constructor.
		 * @param Scene to archive with.
		 * @param Resource manager to use for certain handles.
		 * @param Archive to read from/write to.
		 * @note The archive must not be mullptr.
		 */
		ComponentArchive(Scene* scene, ResourceManager* resource_manager, Archive* archive);

		/**
		 * Constructor.
		 * @param Scene to archive with.
		 * @param Resource manager to use for certain handles.
		 * @param Whether the archive is in write mode or not.
		 */
		ComponentArchive(Scene* scene, ResourceManager* resource_manager, bool writing);

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
			return m_archive ? m_archive->is_writing() : m_writing;
		}
		
		/**
		 * Set the name of the component.
		 * @param Component name.
		 */
		void set_name(const std::string& name)
		{
			m_name = name;
		}

		/**
		 * Get the name of the component.
		 * @return Component name.
		 */
		std::string get_name() const
		{
			return m_name;
		}

		/**
		 * Either register a new field or read a field from the archive.
		 * @tparam Field type.
		 * @param Field name.
		 * @param Pointer to field destination/source.
		 * @note If the archive doesn't exist, the write mode flag will determine whether a read or write occurs.
		 */
		template<typename T>
		void field(const std::string& name, T* data)
		{
			if (m_archive)
			{
				// Set the field
				set_field<T>(name, data);

				if (m_archive->is_writing())
					// Write field to archive
					m_archive->write<T>(*data);
				else
					// Read from the archive
					*data = m_archive->read<T>();
			}
			else
			{
				if(m_writing)
					// Create the field
					set_field<T>(name, data);
				else
				{
					// Set the field if it exists
					Field field = get_field(name);
					if (field.data) *data = *static_cast<T*>(field.data);
				}
			}
		}

		/**
		 * Either register a new field or read a field from the archive.
		 * @tparam Template class type.
		 * @tparam Type held in the template class.
		 * @tparam Allocator.
		 * @param Field name.
		 * @param Pointer to destination/source field.
		 * @note The archive must exist.
		 */
		template<template<class, class> class V, class T, class A>
		void field(const std::string& name, V<T, A>* data)
		{
			if (m_archive)
			{
				if (m_archive->is_writing())
					// Write field to archive
					m_archive->write<V, T, A>(*data);
				else
					// Read feild from archive
					*data = m_archive->read<V, T, A>();
			}
			else
			{
				if (m_writing)
				{
					// Create the field
					set_field<V, T, A>(name, data);
				}
				else
				{
					// Set the field if it exists
					Field field = get_field(name);
					if (field.data) *data = *static_cast<V<T, A>*>(field.data);
				}
			}
		}

		/**
		 * Set a field.
		 * @tparam Field type.
		 * @param Field name.
		 * @param Pointer to field.
		 */
		template<typename T>
		void set_field(const std::string& name, T* data)
		{
			// Make sure the field doesn't already exist
			dk_assert(!field_exists(name));

			// Add field
			m_fields.push_back({});
			m_fields[m_fields.size() - 1].name = name;
			m_fields[m_fields.size() - 1].size = sizeof(T);
			m_fields[m_fields.size() - 1].data = static_cast<void*>(data);
			m_fields[m_fields.size() - 1].type_id = TypeID<T>::id();
		}

		/**
		 * Set a field.
		 * @tparam Template class type.
		 * @tparam Type held in the template class.
		 * @tparam Allocator.
		 * @param Field name.
		 * @param Pointer to field.
		 */
		template<template<class, class> class V, class T, class A>
		void set_field(const std::string& name, V<T, A>* data)
		{
			// Make sure the field doesn't already exist
			dk_assert(!field_exists(name));

			// Add field
			m_fields.push_back({});
			m_fields[m_fields.size() - 1].name = name;
			m_fields[m_fields.size() - 1].size = sizeof(V<T, A>);
			m_fields[m_fields.size() - 1].data = static_cast<void*>(data);
			m_fields[m_fields.size() - 1].type_id = TypeID<V<T, A>>::id();
		}

		/**
		 * Get a field.
		 * @param Field name.
		 * @return Field.
		 */
		Field get_field(const std::string& name)
		{
			for (auto& field : m_fields)
				if (field.name == name)
					return field;

			return Field();
		}

		/**
		 * Get a list of all fields.
		 * @return All fields.
		 */
		const std::vector<Field>& get_fields() const
		{
			return m_fields;
		}

	private:

		/**
		 * Check if a field exists.
		 * @param Field name.
		 * @return If the field exists.
		 */
		bool field_exists(const std::string& name);



		/** Is the archive writing or not? */
		bool m_writing;

		/** Archive we are reading/writing from/to. */
		Archive* m_archive;

		/** Scene. */
		Scene* m_scene;

		/** Resource manager. */
		ResourceManager* m_resource_manager;

		/** The components name. */
		std::string m_name;

		/** Fields. */
		std::vector<Field> m_fields;
	};

	// Specializations

	template<>
	void ComponentArchive::field<Handle<Mesh>>(const std::string& name, Handle<Mesh>* data);

	template<>
	void ComponentArchive::field<Handle<MaterialShader>>(const std::string& name, Handle<MaterialShader>* data);

	template<>
	void ComponentArchive::field<Handle<Material>>(const std::string& name, Handle<Material>* data);

	template<>
	void ComponentArchive::field<Handle<SkyBox>>(const std::string& name, Handle<SkyBox>* data);

	template<>
	void ComponentArchive::field<Handle<CubeMap>>(const std::string& name, Handle<CubeMap>* data);
}