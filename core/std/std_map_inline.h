#ifndef __core_std_map_inline_h_included_
#define __core_std_map_inline_h_included_

template<typename T, u32 HeapPageSize, typename BasicAllocator>
template<typename U, u32 UHeapPageSize, typename UBasicAllocator>
stl_map_alocator<T, HeapPageSize, BasicAllocator>::stl_map_alocator( const stl_map_alocator<U, UHeapPageSize, UBasicAllocator>& ) noexcept
{ }

template<class T, u32 HeapPageSize, typename BasicAllocator>
T* stl_map_alocator<T, HeapPageSize, BasicAllocator>::allocate( std::size_t n )
{
	return heap.allocate( );
}

template<class T, u32 HeapPageSize, typename BasicAllocator>
void stl_map_alocator<T, HeapPageSize, BasicAllocator>::deallocate( T* p, std::size_t n )
{
	heap.deallocate( p );
}


template<class T, u32 HeapPageSize, typename BasicAllocator>
stl_map_alocator<T, HeapPageSize, BasicAllocator>::container_proxy_allocator::container_proxy_allocator( const container_proxy_allocator& ) noexcept
{
	data = nullptr;
}

template<class T, u32 HeapPageSize, typename BasicAllocator>
stl_map_alocator<T, HeapPageSize, BasicAllocator>::container_proxy_allocator::container_proxy_allocator( const stl_map_alocator& all ) noexcept
{
	data = &all.container_proxy_data;
}

template<class T, u32 HeapPageSize, typename BasicAllocator>
std::_Container_proxy* stl_map_alocator<T, HeapPageSize, BasicAllocator>::container_proxy_allocator::allocate( std::size_t n ) noexcept
{
	return (std::_Container_proxy*)data;
}

template<class T, u32 HeapPageSize, typename BasicAllocator>
void stl_map_alocator<T, HeapPageSize, BasicAllocator>::container_proxy_allocator::deallocate( std::_Container_proxy* p, std::size_t n ) noexcept
{ }


template <typename L, typename R, u32 LHeapPageSize, u32 RHeapPageSize, typename LBasicAllocator, typename RBasicAllocator>
bool operator==( const stl_map_alocator<L, LHeapPageSize, LBasicAllocator>&, const stl_map_alocator<R, RHeapPageSize, RBasicAllocator>& ) { return false; }

template <typename L, typename R, u32 LHeapPageSize, u32 RHeapPageSize, typename LBasicAllocator, typename RBasicAllocator>
bool operator!=( const stl_map_alocator<L, LHeapPageSize, LBasicAllocator>&, const stl_map_alocator<R, RHeapPageSize, RBasicAllocator>& ) { return true; }

#endif // #ifndef __core_std_map_inline_h_included_
