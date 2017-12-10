#ifndef __core_std_map_h_included_
#define __core_std_map_h_included_

#include <map>
#include "pool.h"

enum
{
	stl_map_allocator_default_pool_page_size = 4096,
	stl_map_allocator_default_pool_page_max_count = 256
};

template<typename T, uptr PoolPageSize, uptr PoolPageMaxCount>
struct stl_map_alocator
{
public:
	typedef T value_type;

	stl_map_alocator( ) = default;

	template<typename U, uptr UPoolPageSize, uptr UPoolPageMaxCount>
	stl_map_alocator( const stl_map_alocator<U, UPoolPageSize, UPoolPageMaxCount>& ) noexcept;

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
		typedef stl_map_alocator<T, PoolPageSize, PoolPageMaxCount> other;
	};
	
	template<>
	struct rebind<std::_Container_proxy>
	{
		typedef container_proxy_allocator other;
	};

private:
	std::_Container_proxy container_proxy_data;
	dynamic_pool<sizeof(T), PoolPageSize, PoolPageMaxCount> pool;
};

template <typename L, typename R, uptr LPoolPageSize, uptr RPoolPageSize, uptr LPoolPageMaxCount, uptr RPoolPageMaxCount>
bool operator==( const stl_map_alocator<L, LPoolPageSize, LPoolPageMaxCount>&, const stl_map_alocator<R, RPoolPageSize, RPoolPageMaxCount>& );

template <typename L, typename R, uptr LPoolPageSize, uptr RPoolPageSize, uptr LPoolPageMaxCount, uptr RPoolPageMaxCount>
bool operator!=( const stl_map_alocator<L, LPoolPageSize, LPoolPageMaxCount>&, const stl_map_alocator<R, RPoolPageSize, RPoolPageMaxCount>& );


template<typename Key, typename Value, uptr PoolPageSize = stl_map_allocator_default_pool_page_size, uptr PoolPageMaxCount = stl_map_allocator_default_pool_page_max_count>
using map = std::map<Key, Value, std::less<Key>, stl_map_alocator<std::pair<Key, Value>, PoolPageSize, PoolPageMaxCount>>;

#include "std_map_inline.h"

#endif // #ifndef __core_std_map_h_included_
