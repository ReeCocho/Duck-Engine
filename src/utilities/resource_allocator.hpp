#pragma once

/**
 * @file resource_allocator.hpp
 * @brief A resource allocator class.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <vector>
#include "debugging.hpp"

namespace dk
{
	/** ID of a resource in the resource allocator. */
	using ResourceID = uint32_t;

	/**
	 * Base class for resource allocators.
	 */
	class ResourceAllocatorBase
	{
	public:
		
		/**
		 * Destructor.
		 */
		virtual ~ResourceAllocatorBase();

		/**
		 * Check if a resource is allocated.
		 * @param Resource ID.
		 * @return If the resource is allocated.
		 */
		inline bool is_allocated(ResourceID id) const
		{
			dk_assert(id < m_allocation_table.size());
			return m_allocation_table[id];
		}

		/**
		 * Allocate a resource.
		 * @return The resource's ID.
		 */
		virtual ResourceID allocate() = 0;

		/**
		 * Allocate a specific resource.
		 * @param Resource ID.
		 */
		virtual void allocate_by_id(ResourceID id) = 0;

		/**
		 * Deallocate a resource.
		 * @param The resource's ID.
		 */
		virtual void deallocate(ResourceID id) = 0;

		/**
		 * Change the number of resources that can be allocated.
		 * @param New number.
		 */
		virtual void resize(size_t amt) = 0;

		/**
		 * Get the number of resources allocated.
		 * @return Number of resources allocated.
		 */
		inline size_t num_allocated() const
		{
			size_t count = 0;

			for (auto is_alloc : m_allocation_table)
				count += is_alloc ? 1 : 0;

			return count;
		}

		/**
		 * Get the max number of resources that can be allocated.
		 * @return Max number of resources that can be allocated.
		 */
		inline size_t max_allocated() const
		{
			return m_allocation_table.size();
		}

	protected:

		/**
		 * Constructor.
		 * @param Amount of resources preallocated.
		 */
		ResourceAllocatorBase(size_t amt);

		/**
		 * Copy constructor.
		 * @param Other resource allocator.
		 */
		ResourceAllocatorBase(const ResourceAllocatorBase& other) = default;
		
		/**
		 * Assignment operator.
		 */
		ResourceAllocatorBase& operator=(const ResourceAllocatorBase& other) = default;



		/** Allocation table. */
		std::vector<bool> m_allocation_table;
	};



	/**
	 * Resource allocator.
	 * @tparam Type of data the resource allocator allocates.
	 */
	template<class T>
	class ResourceAllocator : public ResourceAllocatorBase
	{
	public:

		/**
		 * Constructor.
		 * @param Number of resources to preallocate.
		 */
		ResourceAllocator(size_t amt) : ResourceAllocatorBase(amt), m_resources(amt) {}

		/**
		 * Destructor.
		 */
		~ResourceAllocator() {}

		/**
		 * Allocate a resource.
		 * @return Resource ID.
		 */
		ResourceID allocate() override
		{
			for (ResourceID i = 0; i < m_allocation_table.size(); ++i)
				if (!m_allocation_table[i])
				{
					m_allocation_table[i] = true;
					return i;
				}

			dk_err("Unable to allocate a new resource.");
			return 0;
		}

		/**
		 * Allocate a specific resource.
		 * @param Resource ID.
		 */
		void allocate_by_id(ResourceID id) override
		{
			dk_assert(id < m_allocation_table.size());
			dk_assert(!is_allocated(id));
			m_allocation_table[id] = true;
		}

		/**
		 * Deallocate a resource.
		 * @param Resource ID.
		 */
		void deallocate(ResourceID id) override
		{
			dk_assert(id < m_allocation_table.size() && is_allocated(id));
			m_allocation_table[id] = false;
		}

		/**
		 * Change the number of resources that can be allocated.
		 * @param New number.
		 */
		void resize(size_t amt) override
		{
			m_allocation_table.resize(amt);
			m_resources.resize(amt);
		}

		/**
		 * Get a resource by it's handle.
		 * @param Handle.
		 * @return Resource.
		 */
		T* get_resource_by_handle(ResourceID id)
		{
			dk_assert(id < m_allocation_table.size() && is_allocated(id));
			return &m_resources.at(id);
		}

	private:

		/**
		 * Copy constructor.
		 * @param Other resource allocator.
		 */
		ResourceAllocator(const ResourceAllocator<T>& other) = default;
		
		/**
		 * Assignment operator.
		 */
		ResourceAllocator<T>& operator=(const ResourceAllocator<T>& other) = default;



		/** Resources. */
		std::vector<T> m_resources;
	};



	/**
	 * Handle for a resource in a resource allocator.
	 * @tparam Type of resource.
	 */
	template<class T>
	class Handle
	{
	public:

		/**
		 * Default constructor.
		 */
		Handle() : id(0), allocator(nullptr) {}

		/**
		 * Constructor.
		 * @param Resource ID.
		 * @param Resource allocator
		 */
		Handle(ResourceID a_id, ResourceAllocator<T>* a_allocator) : id(a_id), allocator(a_allocator) {}

		/**
		 * Equivilence operator.
		 * @param Other handle.
		 * @return If the two handles are equal.
		 */
		inline bool operator==(const Handle<T>& other) const
		{
			return id == other.id && allocator == other.allocator;
		}

		/**
		 * Unequivilence operator.
		 * @param Other handle.
		 * @return If the two handles are not equal.
		 */
		inline bool operator!=(const Handle<T>& other) const
		{
			return id != other.id || allocator != other.allocator;
		}

		/**
		 * Access operator.
		 * @return Resource.
		 */
		inline T* operator->() const
		{
			dk_assert(allocator);
			return allocator->get_resource_by_handle(id);
		}

		/**
		 * Deallocate the resource.
		 */
		inline void deallocate()
		{
			dk_assert(allocator && allocator->is_allocated(id));
			allocator->deallocate(id);
			id = 0;
			allocator = nullptr;
		}

		/**
		 * Determine if the handle is valid.
		 * @return If the handle is valid.
		 */
		inline bool is_valid() const
		{
			return allocator != nullptr && allocator->is_allocated(id);
		}

		/** Resource ID. */
		ResourceID id;

		/** Resource allocator. */
		ResourceAllocator<T>* allocator;
	};
}