#ifndef __core_std_map_inline_h_included_
#define __core_std_map_inline_h_included_

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
template<typename U, uptr UPoolPageSize, uptr UPoolPageMaxCount>
stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::stl_map_alocator( const stl_map_alocator<U, UPoolPageSize, UPoolPageMaxCount>& ) noexcept
{ }

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
T* stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::allocate( std::size_t n )
{
	ASSERT( n == 1 );
	return pool.allocate( sizeof(T) );
}

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
void stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::deallocate( T* p, std::size_t n )
{
	ASSERT( n == 1 );
	pool.deallocate( p );
}


template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::container_proxy_allocator::container_proxy_allocator( const container_proxy_allocator& ) noexcept
{
	data = nullptr;
}

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::container_proxy_allocator::container_proxy_allocator( const stl_map_alocator& all ) noexcept
{
	data = &all.container_proxy_data;
}

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
std::_Container_proxy* stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::container_proxy_allocator::allocate( std::size_t n ) noexcept
{
	return (std::_Container_proxy*)data;
}

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
void stl_map_alocator<T, PoolPageSize, PoolPageMaxCount>::container_proxy_allocator::deallocate( std::_Container_proxy* p, std::size_t n ) noexcept
{ }


template <typename L, typename R, uptr LPoolPageSize, uptr RPoolPageSize, uptr LPoolPageMaxCount, uptr RPoolPageMaxCount>
bool operator==( const stl_map_alocator<L, LPoolPageSize, LPoolPageMaxCount>&, const stl_map_alocator<R, RPoolPageSize, RPoolPageMaxCount>& ) { return false; }

template <typename L, typename R, uptr LPoolPageSize, uptr RPoolPageSize, uptr LPoolPageMaxCount, uptr RPoolPageMaxCount>
bool operator!=( const stl_map_alocator<L, LPoolPageSize, LPoolPageMaxCount>&, const stl_map_alocator<R, RPoolPageSize, RPoolPageMaxCount>& ) { return true; }

#endif // #ifndef __core_std_map_inline_h_included_
