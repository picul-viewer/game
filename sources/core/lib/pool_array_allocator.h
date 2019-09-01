#ifndef GUARD_CORE_POOLSET_H_INCLUDED
#define GUARD_CORE_POOLSET_H_INCLUDED

#include <types.h>
#include <macros.h>
#include "pool_allocator.h"

namespace lib {

template<typename ... TList>
class pool_array_allocator;

template<typename T>
class pool_array_allocator<T> : lib::pool_allocator<sizeof(T)>
{
public:
	using lib::pool_allocator<sizeof(T)>::create;
	using lib::pool_allocator<sizeof(T)>::destroy;
	using lib::pool_allocator<sizeof(T)>::clear;

	template<typename U>
	U* allocate( )
	{
		UNREACHABLE_CODE
	}

	template<typename U>
	void deallocate( U* )
	{
		UNREACHABLE_CODE
	}
	
	template<>
	T* allocate<T>( )
	{
		return lib::pool_allocator<sizeof(T)>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* const p )
	{
		lib::pool_allocator<sizeof(T)>::deallocate( p );
	}

	uptr get_object_allocator_index( pointer const object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= lib::pool_allocator<sizeof(T)>::m_data ) &&
				( object <  lib::pool_allocator<sizeof(T)>::m_data + lib::pool_allocator<sizeof(T)>::m_size );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG

		return 0;
	}
};

template<typename T, typename ... TList>
struct pool_array_allocator<T, TList ...> :
	lib::pool_allocator<sizeof(T)>,
	pool_array_allocator<TList ...>
{
public:
	void create( pointer const memory, uptr const size_per_element )
	{
		pointer const memory_ptr = memory ? memory : virtual_allocator( ).allocate( type_count<T, TList ...>::value * size_per_element );

		lib::pool_allocator<sizeof(T)>::create( memory_ptr, size_per_element );
		pool_array_allocator<TList ...>::create( lib::pool_allocator<sizeof(T)>::m_data + size_per_element, size_per_element );
	}

	void destroy( )
	{
		lib::pool_allocator<sizeof(T)>::destroy( );
	}
	
	void clear( )
	{
		lib::pool_allocator<sizeof(T)>::clear( );
		pool_array_allocator<TList ...>::clear( );
	}
	
	template<typename U>
	U* allocate( )
	{
		return pool_array_allocator<TList ...>::allocate<U>( );
	}

	template<typename U>
	void deallocate( U* const p )
	{
		pool_array_allocator<TList ...>::deallocate<U>( p );
	}
	
	template<>
	T* allocate<T>( )
	{
		return lib::pool_allocator<sizeof(T)>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* const p )
	{
		lib::pool_allocator<sizeof(T)>::deallocate( p );
	}
	
	uptr get_object_allocator_index( pointer const object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= lib::pool_allocator<sizeof(T)>::m_data ) &&
				( object <  lib::pool_allocator<sizeof(T)>::m_data + lib::pool_allocator<sizeof(T)>::m_size );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG

		return ( object - lib::pool_allocator<sizeof(T)>::m_data ) / lib::pool_allocator<sizeof(T)>::m_size;
	}
};


template<typename ... TList>
class dynamic_pool_array_allocator;

template<typename T>
class dynamic_pool_array_allocator<T> : lib::dynamic_pool_allocator<sizeof(T)>
{
public:
	using lib::dynamic_pool_allocator<sizeof(T)>::create;
	using lib::dynamic_pool_allocator<sizeof(T)>::destroy;
	using lib::dynamic_pool_allocator<sizeof(T)>::clear;

	template<typename U>
	U* allocate( )
	{
		UNREACHABLE_CODE
	}

	template<typename U>
	void deallocate( U* )
	{
		UNREACHABLE_CODE
	}
	
	template<>
	T* allocate<T>( )
	{
		return lib::dynamic_pool_allocator<sizeof(T)>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* const p )
	{
		lib::dynamic_pool_allocator<sizeof(T)>::deallocate( p );
	}

	uptr get_object_allocator_index( pointer const object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= lib::dynamic_pool_allocator<sizeof(T)>::m_data ) &&
				( object <  lib::dynamic_pool_allocator<sizeof(T)>::m_data + lib::dynamic_pool_allocator<sizeof(T)>::m_size );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG

		return 0;
	}
};

template<typename T, typename ... TList>
struct dynamic_pool_array_allocator<T, TList ...> :
	lib::dynamic_pool_allocator<sizeof(T)>,
	dynamic_pool_array_allocator<TList ...>
{
public:
	void create( pointer const memory, uptr const size_per_element, uptr const initial_size )
	{
		pointer const memory_ptr = memory ? memory : virtual_allocator( ).reserve( nullptr, type_count<T, TList ...>::value * size_per_element );

		lib::dynamic_pool_allocator<sizeof(T)>::create( memory_ptr, size_per_element, initial_size );
		dynamic_pool_array_allocator<TList ...>::create( lib::pool_allocator<sizeof(T)>::m_data + size_per_element, size_per_element, initial_size );
	}

	void destroy( )
	{
		lib::dynamic_pool_allocator<sizeof(T)>::destroy( );
	}
	
	void clear( )
	{
		lib::dynamic_pool_allocator<sizeof(T)>::clear( );
		dynamic_pool_array_allocator<TList ...>::clear( );
	}
	
	template<typename U>
	U* allocate( )
	{
		return dynamic_pool_array_allocator<TList ...>::allocate<U>( );
	}

	template<typename U>
	void deallocate( U* const p )
	{
		dynamic_pool_array_allocator<TList ...>::deallocate<U>( p );
	}
	
	template<>
	T* allocate<T>( )
	{
		return lib::dynamic_pool_allocator<sizeof(T)>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* const p )
	{
		lib::dynamic_pool_allocator<sizeof(T)>::deallocate( p );
	}
	
	uptr get_object_allocator_index( pointer const object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= lib::dynamic_pool_allocator<sizeof(T)>::m_data ) &&
				( object <  lib::dynamic_pool_allocator<sizeof(T)>::m_data + lib::dynamic_pool_allocator<sizeof(T)>::m_size );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG

		return ( object - lib::dynamic_pool_allocator<sizeof(T)>::m_data ) / lib::dynamic_pool_allocator<sizeof(T)>::m_size;
	}
};

} // namespace lib

#endif // #ifndef GUARD_CORE_POOLSET_H_INCLUDED