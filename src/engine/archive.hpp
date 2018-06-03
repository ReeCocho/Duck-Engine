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
		void field(T* data)
		{
			if (m_archive->is_writing())
				// Write field to archive
				m_archive->write<T>(*data);
			else
				// Read from the archive
				*data = m_archive->read<T>();
		}

		/**
		 * Either register a new field or read a field from the archive.
		 * @tparam Template class type.
		 * @tparam Type held in the template class.
		 * @tparam Allocator.
		 * @param Pointer to destination/source field.
		 */
		template<template<class, class> class V, class T, class A>
		void field(V<T, A>* data)
		{
			if (m_archive->is_writing())
				// Write field to archive
				m_archive->write<V, T, A>(*data);
			else
				// Read feild from archive
				*data = m_archive->read<V, T, A>();
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
	void ComponentArchive::field<HMesh>(HMesh* data);

	template<>
	void ComponentArchive::field<HMaterialShader>(HMaterialShader* data);

	template<>
	void ComponentArchive::field<HMaterial>(HMaterial* data);

	template<>
	void ComponentArchive::field<HSkyBox>(HSkyBox* data);

	template<>
	void ComponentArchive::field<HCubeMap>(HCubeMap* data);
}