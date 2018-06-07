/**
 * @file resource_allocator.cpp
 * @brief Resource allocator source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "resource_allocator.hpp"

namespace dk
{
	ResourceAllocatorBase::ResourceAllocatorBase(size_t amt) : m_allocation_table(amt) {}

	ResourceAllocatorBase::~ResourceAllocatorBase() {}
}