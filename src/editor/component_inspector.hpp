#pragma once

/**
 * @file component_inspector.hpp
 * @brief Component inspector header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <ecs\scene.hpp>
#include <vector>
#include <functional>
#include <utilities\reflection.hpp>
#include <utilities\archive.hpp>
#include <engine\resource_manager.hpp>

namespace dk
{
	/**
	 * Used by the editor to inspect a component.
	 */
	class ComponentInspector : public ReflectionContext
	{
	public:

		/**
		 * Inspector mode enum.
		 */
		enum class Mode
		{
			Name = 0,		// Expecting component name
			Field = 1,		// Expecting a field
			Enum = 2		// Expecting an enum value
		};

		/**
		 * Field types.
		 */
		enum class FieldType
		{
			Data = 0,
			Vector = 1,
			Enum = 2
		};

		/**
		 * Standard component inspector field.
		 */
		class Field
		{
		public:

			Field() : field_type(FieldType::Data) {}

			virtual ~Field() {}

			/** Name. */
			std::string name = "";

			/** Pointer to field data. */
			void* data = nullptr;

			/** Callback should you choose to change the data pointer. */
			std::function<void()> callback = [] {};

			/** Field type ID. */
			size_t type_id = 0;

			/** Field type. */
			FieldType field_type;
		};

		/**
		 * Vector component inspector field.
		 * @note The type id represents the type of element stored in the vector.
		 */
		class VectorField : public Field
		{
		public:

			VectorField() { field_type = FieldType::Vector; }

			/** Number of elements in the field. */
			size_t list_size = 0;
		};

		/**
		 * Enum component inspector field.
		 */
		class EnumField : public Field
		{
		public:

			EnumField() { field_type = FieldType::Enum; }

			/** Enum options. */
			std::vector<std::tuple<size_t, std::string>> enum_options = {};
		};

		/**
		 * Constructor.
		 * @param Scene to archive with.
		 * @param Resource manager to use for certain handles.
		 */
		ComponentInspector(Scene* scene, ResourceManager* resource_manager);

		/**
		 * Destructor.
		 */
		~ComponentInspector();

		/**
		 * Set the name of the component.
		 * @param Component name.
		 */
		void set_name(const std::string& name);

		/**
		 * Get the name of the component.
		 * @return Component name.
		 */
		std::string get_name() const
		{
			return m_name;
		}

		/**
		 * Register a field.
		 * @tparam Field type.
		 * @param Field name.
		 * @param Pointer to field source.
		 * @param Optional callback.
		 */
		template<typename T>
		void set_field(const std::string& name, T* data, std::function<void()> callback = [] {})
		{
			dk_assert(m_mode == Mode::Field && !field_exists(name));

			// Add field
			Field* field = new Field();
			field->name = name;
			field->callback = callback;
			field->type_id = TypeID<T>::id();
			field->data = data;

			m_fields.push_back(field);
		}

		/**
		 * Register a vector field.
		 * @tparam Vector field type.
		 * @param Field name.
		 * @param Pointer to field source.
		 * @param Optional callback.
		 */
		template<typename T>
		void set_field(const std::string& name, std::vector<T>* data, std::function<void()> callback = [] {})
		{
			dk_assert(m_mode == Mode::Field && !field_exists(name));

			// Add field
			VectorField* field = new VectorField();
			field->name = name;
			field->callback = callback;
			field->type_id = TypeID<T>::id();
			field->data = data;
			field->list_size = data->size();

			m_fields.push_back(field);
		}

		/**
		 * Begin recording an enum.
		 * @tparam Enum type.
		 * @param Field name.
		 * @param Pointer to the field source.
		 * @param Optional callback.
		 */
		template<typename T>
		void begin_enum(const std::string& name, T* data, std::function<void()> callback = [] {})
		{
			dk_assert(m_mode == Mode::Field && !field_exists(name));
			m_mode = Mode::Enum;

			// Add field
			EnumField* field = new EnumField();
			field->name = name;
			field->callback = callback;
			field->type_id = TypeID<T>::id();
			field->data = data;
			
			m_fields.push_back(field);
		}

		/**
		 * Record an enum value.
		 * @tparam Enum type.
		 * @param Enum value name.
		 * @param Enum value.
		 */
		template<typename T>
		void set_enum(const std::string& name, T data)
		{
			dk_assert(m_mode == Mode::Enum);

			// Record the value
			auto* field = static_cast<EnumField*>(m_fields[m_fields.size() - 1]);
			field->enum_options.push_back(std::make_tuple(static_cast<size_t>(data), name));
		}

		/**
		 * End recording enum values.
		 */
		void end_enum()
		{
			dk_assert(m_mode == Mode::Enum);
			m_mode = Mode::Field;
		}

		/**
		 * Get field.
		 * @return Fields.
		 */
		const std::vector<Field*>& get_fields() const
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

		/** Scene. */
		Scene * m_scene;

		/** Resource manager. */
		ResourceManager* m_resource_manager;

		/** Current inspector mode. */
		Mode m_mode;

		/** The components name. */
		std::string m_name;

		/** Fields. */
		std::vector<Field*> m_fields;
	};
}