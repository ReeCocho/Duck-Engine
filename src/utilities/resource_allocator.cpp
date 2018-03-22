/**
 * @file resource_allocator.cpp
 * @brief Resource allocator source.
 * @author Connor J. Bramham (ReeCocho)
 */

#include "resource_allocator.hpp"

namespace dk
{
	ResourceAllocatorBase::ResourceAllocatorBase(size_t amt) : m_allocation_table(amt)
	{

	}

	ResourceAllocatorBase::~ResourceAllocatorBase()
	{

	}

	size_t ResourceAllocatorBase::num_allocated() const
	{
		size_t count = 0;

		for (auto is_alloc : m_allocation_table)
			count += is_alloc ? 1 : 0;

		return count;
	}
}