#ifndef __core_std_map_h_included_
#define __core_std_map_h_included_

#include <map>
#include "heap.h"

enum { stl_map_allocator_default_heap_page_size = 4096 };

template<class T, u32 HeapPageSize, typename BasicAllocator>
struct stl_map_alocator
{
public:
	typedef T value_type;

	stl_map_alocator( ) = default;

	template<typename U, u32 UHeapPageSize, typename UBasicAllocator>
	stl_map_alocator( const stl_map_alocator<U, UHeapPageSize, UBasicAllocator>& ) noexcept;

	T* allocate( std::size_t n );
	void deallocate( T* p, std::size_t n );

private:
	// STL map is need one _Container_proxy per instance
	// lets keep it in static memory!
	struct container_proxy_allocator
	{
		typedef std::_Container_proxy value_type;

		container_proxy_allocator() = default;
		
		container_proxy_allocator( const container_proxy_allocator& ) noexcept;
		container_proxy_allocator( const stl_map_alocator& all ) noexcept;

		std::_Container_proxy* allocate( std::size_t n ) noexcept;
		void deallocate( std::_Container_proxy* p, std::size_t n ) noexcept;

	private:
		const std::_Container_proxy* data;
	};
	
public:
	template<typename T>
	struct rebind
	{
		typedef stl_map_alocator<T, HeapPageSize, BasicAllocator> other;
	};
	
	template<>
	struct rebind<std::_Container_proxy>
	{
		typedef container_proxy_allocator other;
	};

private:
	std::_Container_proxy container_proxy_data;
	dynamic_array_heap<sizeof(T), HeapPageSize, BasicAllocator> heap;
};

template <typename L, typename R, u32 LHeapPageSize, u32 RHeapPageSize, typename LBasicAllocator, typename RBasicAllocator>
bool operator==( const stl_map_alocator<L, LHeapPageSize, LBasicAllocator>&, const stl_map_alocator<R, RHeapPageSize, RBasicAllocator>& );

template <typename L, typename R, u32 LHeapPageSize, u32 RHeapPageSize, typename LBasicAllocator, typename RBasicAllocator>
bool operator!=( const stl_map_alocator<L, LHeapPageSize, LBasicAllocator>&, const stl_map_alocator<R, RHeapPageSize, RBasicAllocator>& );


template<typename Key, typename Value, typename BasicAllocator = mem_allocator, u32 HeapPageSize = stl_map_allocator_default_heap_page_size>
using map = std::map<Key, Value, std::less<Key>, stl_map_alocator<std::pair<Key, Value>, HeapPageSize, BasicAllocator>>;

#include "std_map_inline.h"

#endif // #ifndef __core_std_map_h_included_
