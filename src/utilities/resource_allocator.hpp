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
	using ResourceID = size_t;

	/**
	 * @brief Base class for resource allocators.
	 */
	class ResourceAllocatorBase
	{
	public:
		
		/**
		 * @brief Destructor.
		 */
		virtual ~ResourceAllocatorBase();

		/**
		 * @brief Check if a resource is allocated.
		 * @param Resource ID.
		 * @return If the resource is allocated.
		 */
		bool is_allocated(ResourceID id) const
		{
			dk_assert(id < m_allocation_table.size());
			return m_allocation_table[id];
		}

		/**
		 * @brief Allocate a resource.
		 * @return The resource's ID.
		 */
		virtual ResourceID allocate() = 0;

		/**
		 * @brief Deallocate a resource.
		 * @param The resource's ID.
		 */
		virtual void deallocate(ResourceID id) = 0;

		/**
		 * @brief Change the number of resources that can be allocated.
		 * @param New number.
		 */
		virtual void resize(size_t amt) = 0;

		/**
		 * @brief Get the number of resources allocated.
		 * @return Number of resources allocated.
		 */
		size_t num_allocated() const;

		/**
		 * @brief Get the max number of resources that can be allocated.
		 * @return Max number of resources that can be allocated.
		 */
		size_t max_allocated() const
		{
			return m_allocation_table.size();
		}

	protected:

		/**
		 * @brief Constructor.
		 * @param Amount of resources preallocated.
		 */
		ResourceAllocatorBase(size_t amt);

		/**
		 * @brief Copy constructor.
		 * @param Other resource allocator.
		 */
		ResourceAllocatorBase(const ResourceAllocatorBase& other) = default;
		
		/**
		 * @brief Assignment operator.
		 */
		ResourceAllocatorBase& operator=(const ResourceAllocatorBase& other) = default;



		/** Allocation table. */
		std::vector<bool> m_allocation_table;
	};



	/**
	 * @brief Resource allocator.
	 * @tparam Type of data the resource allocator allocates.
	 */
	template<class T>
	class ResourceAllocator : public ResourceAllocatorBase
	{
	public:

		/**
		 * @brief Constructor.
		 * @param Number of resources to preallocate.
		 */
		ResourceAllocator(size_t amt) : ResourceAllocatorBase(amt), m_resources(amt) {}

		/**
		 * @brief Destructor.
		 */
		~ResourceAllocator() {}

		/**
		 * @brief Allocate a resource.
		 * @return Resource ID.
		 */
		ResourceID allocate() override
		{
			size_t free_id = 0;
			bool found_id = false;

			for (size_t i = 0; i < m_allocation_table.size(); ++i)
				if (!m_allocation_table[i])
				{
					free_id = i;
					found_id = true;
					break;
				}

			dk_assert(found_id);
			m_allocation_table[free_id] = true;
			return free_id;
		}

		/**
		 * @brief Deallocate a resource.
		 * @param Resource ID.
		 */
		void deallocate(ResourceID id) override
		{
			dk_assert(id < m_allocation_table.size() && m_allocation_table[id]);
			m_allocation_table[id] = false;
			m_resources[id].~T();
		}

		/**
		 * @brief Change the number of resources that can be allocated.
		 * @param New number.
		 */
		void resize(size_t amt) override
		{
			m_allocation_table.resize(amt);
			m_resources.resize(amt);
		}

		/**
		 * @brief Get a resource by it's handle.
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
		 * @brief Copy constructor.
		 * @param Other resource allocator.
		 */
		ResourceAllocator(const ResourceAllocator<T>& other) = default;
		
		/**
		 * @brief Assignment operator.
		 */
		ResourceAllocator<T>& operator=(const ResourceAllocator<T>& other) = default;



		/** Resources. */
		std::vector<T> m_resources;
	};



	/**
	 * @brief Handle for a resource in a resource allocator.
	 * @tparam Type of resource.
	 */
	template<class T>
	class Handle
	{
	public:

		/**
		 * @brief Default constructor.
		 */
		Handle() : id(0), allocator(nullptr) {}

		/**
		 * @brief Constructor.
		 * @param Resource ID.
		 * @param Resource allocator
		 */
		Handle(ResourceID a_id, ResourceAllocator<T>* a_allocator) : id(a_id), allocator(a_allocator) {}

		/**
		 * @brief Equivilence operator.
		 * @param Other handle.
		 * @return If the two handles are equal.
		 */
		bool operator==(const Handle<T>& other) const
		{
			return id == other.id && allocator == other.allocator;
		}

		/**
		 * @brief Unequivilence operator.
		 * @param Other handle.
		 * @return If the two handles are not equal.
		 */
		bool operator!=(const Handle<T>& other) const
		{
			return id != other.id || allocator != other.allocator;
		}

		/**
		 * @brief Access operator.
		 * @return Resource.
		 */
		T* operator->()
		{
			dk_assert(allocator);
			return allocator->get_resource_by_handle(id);
		}

		/** Resource ID. */
		ResourceID id = 0;

		/** Resource allocator. */
		ResourceAllocator<T>* allocator = nullptr;
	};
}