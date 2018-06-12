#pragma once

/**
 * @file reflection.hpp
 * @brief Reflection header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <tuple>
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include "resource_allocator.hpp"
#include "debugging.hpp"

namespace dk
{
	// Used for checking if types are specializations of other types

	template<typename Test, template<typename...> class Ref>
	struct is_specialization : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

	/**
	 * A template class with a static member to get IDs.
	 */
	template<class T>
	class TypeIDGenerator final
	{
	public:

		static const char id = 0;
	};

	/** Duck type ID */
	using type_id = intptr_t;

	/**
	 * A template class to get an ID.
	 */
	template<class T>
	class TypeID final
	{
	public:

		static constexpr inline type_id id()
		{
			return reinterpret_cast<type_id>(&TypeIDGenerator<T>::id);
		}
	};

	/**
	 * Base class for reflection contexts.
	 */
	class ReflectionContext
	{
	public:

		/**
		 * Field types.
		 */
		enum class FieldType
		{
			Variable = 0,
			Vector = 1,
			Enum = 2,
			Handle = 3
		};

		/**
		 * Field of a class.
		 */
		class Field
		{
		public:

			/**
			 * Constructor.
			 */
			Field() {}

			/**
			 * Destructor.
			 */
			virtual ~Field() = default;

			/** Field name. */
			std::string name = "";

			/** Field type. */
			FieldType type = FieldType::Variable;

			/** Field type ID. */
			type_id type_id = 0;

			/** Field data. */
			void* data = nullptr;

			/** Size of field data in bytes. */
			uint32_t data_size = 0;

			/** Callback to be used when the field is changed. */
			std::function<void()> callback = [] {};
		};

		/**
		 * Enumeration field of a class.
		 */
		class EnumField : public Field
		{
		public:

			typedef std::tuple<uint64_t, std::string> EnumValue;

			/**
			 * Constructor.
			 */
			EnumField() : enum_values({})
			{
				name = ""; 
				type = FieldType::Enum;
				type_id = 0; 
				data = nullptr; 
				data_size = 0;
				callback = [] {};
			}

			/** List of enumation fields. */
			std::vector<EnumValue> enum_values;
		};

		/**
		 * Handle field of a class.
		 */
		class HandleField : public Field
		{
		public:

			/**
			 * Constructor.
			 */
			HandleField() : 
				resource_type(0), 
				resource_id(0),
				null_handle(false)
			{
				name = "";
				type = FieldType::Handle;
				type_id = 0;
				data = nullptr;
				data_size = 0;
				callback = [] {};
			}

			/** Type ID of resources referenced by the handle. */
			dk::type_id resource_type;

			/** Resource ID. */
			ResourceID resource_id;

			/** If the handle is a null handle. */
			bool null_handle;
		};

		/**
		 * Vector field of a class.
		 */
		class VectorField : public Field
		{
		public:

			/**
			 * Constructor.
			 */
			VectorField() : elements({})
			{
				name = ""; 
				type = FieldType::Vector; 
				type_id = 0; 
				data = nullptr; 
				data_size = 0;
				callback = [] {};
			}

			/** Resize the vector. */
			std::function<void(size_t)> resize = [](size_t i) {};

			/** Get an element of the vector. */
			std::function<std::shared_ptr<Field>(size_t)> get_element = [](size_t i) { return nullptr; };

			/** Field descriptions of the vectors elements. */
			std::vector<std::shared_ptr<Field>> elements;
		};

		/**
		 * Constructor.
		 */
		ReflectionContext();

		/**
		 * Destructor.
		 */
		virtual ~ReflectionContext() = default;

		/**
		 * Set the name of the class.
		 * @param Class name.
		 */
		inline void set_name(const std::string& name)
		{
			m_name = name;
		}
		
		/**
		 * Get the name of the class.
		 * @return Class name.
		 */
		inline std::string get_name() const
		{
			return m_name;
		}

		/**
		 * Set a field.
		 * @tparam Field type.
		 * @param Field name.
		 * @param Field data.
		 * @param Optional callback to be called when the field is changed.
		 */
		template<typename T>
		void set_field(const std::string& name, T& field, std::function<void()> callback = [] {})
		{
			dk_assert(!field_exists(name));
			m_fields.push_back(create_field(name, field, callback));
		}

		/**
		 * Set a vector field.
		 * @tparam Type held by the vector.
		 * @param Field name.
		 * @param Vector field data.
		 * @param Optional callback to be called when the field is changed.
		 */
		template<typename T>
		void set_field(const std::string& name, std::vector<T>& field, std::function<void()> callback = [] {})
		{
			dk_assert(!field_exists(name));
			m_fields.push_back(create_field(name, field, callback));
		}

		/**
		 * Set a handle field.
		 * @tparam Type of resource held by the handle.
		 * @param Field name.
		 * @param Field data.
		 * @param Optional callback to be called when the field is changed.
		 */
		template<typename T>
		void set_field(const std::string& name, Handle<T>& field, std::function<void()> callback = [] {})
		{
			dk_assert(!field_exists(name));
			m_fields.push_back(create_field(name, field, callback));
		}

		/**
		 * Get a field by name.
		 * @param Field name.
		 * @return Field.
		 * @note Will return nullptr if the filed could not be found
		 */
		Field* get_field(const std::string& name)
		{
			for (size_t i = 0; i < m_fields.size(); ++i)
				if (m_fields[i]->name == name)
					return m_fields[i].get();

			return nullptr;
		}

		/**
		 * Get a list of all fields.
		 * @return All fields.
		 */
		const std::vector<std::shared_ptr<Field>>& get_fields() const
		{
			return m_fields;
		}

	protected:

		/**
		 * Check if a field exists.
		 * @param Field name.
		 * @return If the field with the given name already exists.
		 */
		bool field_exists(const std::string& name)
		{
			for (size_t i = 0; i < m_fields.size(); ++i)
				if (m_fields[i]->name == name)
					return true;
			return false;
		}

		/**
		 * Create a field.
		 * @tparam Field type.
		 * @param Field name.
		 * @param Field data.
		 * @param Optional callback to be called when the field is changed.
		 * @return New field.
		 */
		template<typename T>
		std::shared_ptr<Field> create_field(const std::string& name, T& data, std::function<void()> callback = [] {})
		{
			std::shared_ptr<Field> field = std::make_shared<Field>();
			field->name = name;
			field->type_id = TypeID<T>::id();
			field->data = (void*)&data;
			field->data_size = sizeof(T);
			field->callback = callback;
			return field;
		}

		/**
		 * Create a vector field.
		 * @tparam Type held by the vector.
		 * @param Field name.
		 * @param Vector field data.
		 * @param Optional callback to be called when the field is changed.
		 * @return Field.
		 */
		template<typename T>
		std::shared_ptr<Field> create_field(const std::string& name, std::vector<T>& data, std::function<void()> callback = [] {})
		{
			static_assert(!is_specialization<T, std::vector>::value, "Cannot use multidimensional vectors.");

			std::shared_ptr<VectorField> field = std::make_shared<VectorField>();
			field->name = name;
			field->type_id = TypeID<std::vector<T>>::id();
			field->data = (void*)&data;
			field->data_size = sizeof(std::vector<T>);
			field->callback = callback;
			field->resize = [this, &data](size_t i) { data.resize(i); };
			field->get_element = [this, &data, callback](size_t i) { return create_field("", data[i], callback); };

			field->elements.resize(data.size());
			for (size_t i = 0; i < data.size(); ++i)
				field->elements[i] = create_field("", data[i]);

			return field;
		}

		/**
		 * Create a enum field.
		 * @tparam Enum field type.
		 * @param Field name.
		 * @param Field data.
		 * @param Optional callback to be called when the field is changed.
		 * @return Field.
		 */
		template<typename T>
		std::shared_ptr<Field> create_enum_field(const std::string& name, T& data, std::function<void()> callback = [] {})
		{
			std::shared_ptr<EnumField> field = std::make_shared<EnumField>();
			field->name = name;
			field->type_id = TypeID<T>::id();
			field->data = (void*)&data;
			field->data_size sizeof(T);
			field->callback = callback;
			return field;
		}

		/**
		 * Create a handle field.
		 * @tparam Type of resource held by the handle.
		 * @param Field name.
		 * @param Field data.
		 * @param Optional callback to be called when the field is changed.
		 * @return Field.
		 */
		template<typename T>
		std::shared_ptr<Field> create_field(const std::string& name, Handle<T>& data, std::function<void()> callback = [] {})
		{
			auto field = std::make_shared<HandleField>();
			field->name = name;
			field->type_id = TypeID<Handle<T>>::id();
			field->data = (void*)&data;
			field->data_size = sizeof(Handle<T>);
			field->callback = callback;
			field->resource_type = TypeID<T>::id();
			field->resource_id = data.id;
			field->null_handle = data == Handle<T>() ? true : false;
			return field;
		}

		/** Name of the reflected class. */
		std::string m_name;
		
		/** List of fields. */
		std::vector<std::shared_ptr<Field>> m_fields;
	};
}