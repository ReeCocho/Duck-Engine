#pragma once

/**
 * @file reflection.hpp
 * @brief Reflection header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */

namespace dk
{
	/**
	 * TypeIDGenerator
	 * @brief A template class with a static member to get IDs.
	 */
	template<class T>
	class TypeIDGenerator final
	{
	public:

		static const char id = 0;
	};

	/**
	 * TypeID
	 * @brief A template class to get ID.
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
}