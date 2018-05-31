#pragma once

/**
 * @file reflection.hpp
 * @brief Reflection header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */

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

	/**
	 * A template class to get an ID.
	 */
	template<class T>
	class TypeID final
	{
	public:

		static constexpr inline size_t id()
		{
			return reinterpret_cast<size_t>(&TypeIDGenerator<T>::id);
		}
	};

	/**
	 * Base class for reflection contexts.
	 * @note All ReflectionContext's should follow the form context->field<TYPE>(name_of_field, field_of_TYPE);
	 */
	class ReflectionContext
	{
	public:

		/**
		 * Constructor.
		 */
		ReflectionContext() = default;

		/**
		 * Destructor.
		 */
		virtual ~ReflectionContext() = default;
	};
}