#pragma once

/**
 * @file reflection.hpp
 * @brief Reflection header file.
 * @author Connor J. Bramham (ReeCocho)
 */

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
		 * Constructor.
		 */
		ReflectionContext() = default;

		/**
		 * Destructor.
		 */
		virtual ~ReflectionContext() = default;
	};
}