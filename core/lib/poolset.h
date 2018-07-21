#ifndef __core_poolset_h_included_
#define __core_poolset_h_included_

#include <types.h>
#include <macros.h>
#include "pool.h"

template<template<uptr, uptr> class Base, uptr PageSize, typename ... TList>
struct base_poolset;

template<template<uptr, uptr> class Base, uptr PageSize, typename T>
struct base_poolset<Base, PageSize, T> : Base<sizeof(T), PageSize / sizeof(T)>
{
public:
	using Base<sizeof(T), PageSize / sizeof(T)>::create;
	using Base<sizeof(T), PageSize / sizeof(T)>::destroy;
	using Base<sizeof(T), PageSize / sizeof(T)>::clear;

	template<typename U>
	U* allocate( )
	{
		ASSERT( false );
	}

	template<typename U>
	void deallocate( U* p )
	{
		ASSERT( false );
	}
	
	template<>
	T* allocate<T>( )
	{
		return Base<sizeof(T), PageSize / sizeof(T)>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::deallocate( p );
	}

	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize / sizeof(T)>::m_data ) &&
				( object <  Base<sizeof(T), PageSize / sizeof(T)>::m_data + pool_page_size_helper<PageSize>::value );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return 0;
	}
};

template<template<uptr, uptr> class Base, uptr PageSize, typename T, typename ... TList>
struct base_poolset<Base, PageSize, T, TList ...> :
	Base<sizeof(T), PageSize / sizeof(T)>,
	base_poolset<Base, PageSize, TList ...>
{
public:
	void create( )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::create( virtual_mem_allocator( ).commit( nullptr, pool_page_size_helper<PageSize>::value * type_count<T, TList ...>::value ) );
		base_poolset<Base, PageSize, TList ...>::create( Base<sizeof(T), PageSize / sizeof(T)>::m_data + pool_page_size_helper<PageSize>::value );
	}

	void destroy( )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::destroy( );
		base_poolset<Base, PageSize, TList ...>::destroy( );
	}
	
	void clear( )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::clear( );
		base_poolset<Base, PageSize, TList ...>::clear( );
	}
	
	template<typename U>
	U* allocate( )
	{
		return base_poolset<Base, PageSize, TList ...>::allocate<U>( );
	}

	template<typename U>
	void deallocate( U* p )
	{
		base_poolset<Base, PageSize, TList ...>::deallocate<U>( p );
	}
	
	template<>
	T* allocate<T>( )
	{
		return Base<sizeof(T), PageSize / sizeof(T)>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::deallocate( p );
	}
	
	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize / sizeof(T)>::m_data ) &&
				( object <  Base<sizeof(T), PageSize / sizeof(T)>::m_data + pool_page_size_helper<PageSize>::value * type_count<T, TList ...>::value );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return (u32)( ( object - Base<sizeof(T), PageSize / sizeof(T)>::m_data ) / pool_page_size_helper<PageSize>::value );
	}

protected:
	void create( pointer memory )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::create( memory );
		base_poolset<Base, PageSize, TList ...>::create( memory + pool_page_size_helper<PageSize>::value );
	}
};


template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename ... TList>
struct base_dynamic_poolset;

template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename T>
struct base_dynamic_poolset<Base, PageSize, PageMaxCount, T> : Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>
{
public:
	using Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::create;
	using Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::destroy;
	using Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::clear;

	template<typename U>
	U* allocate( )
	{
		ASSERT( false );
	}

	template<typename U>
	void deallocate( U* p )
	{
		ASSERT( false );
	}
	
	template<>
	T* allocate<T>( )
	{
		return Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::deallocate( p );
	}
	
	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::m_data ) &&
				( object <  Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::m_data + pool_page_size_helper<PageSize>::value * PageMaxCount );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return 0;
	}
};

template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename T, typename ... TList>
struct base_dynamic_poolset<Base, PageSize, PageMaxCount, T, TList ...> :
	Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>,
	base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>
{
public:
	void create( )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::create( virtual_mem_allocator( ).reserve( nullptr, pool_page_size_helper<PageSize>::value * PageMaxCount * type_count<T, TList ...>::value ) );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::create( Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::m_data + pool_page_size_helper<PageSize>::value * PageMaxCount );
	}

	void destroy( )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::destroy( );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::destroy( );
	}
	
	void clear( )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::clear( );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::clear( );
	}
	
	template<typename U>
	U* allocate( )
	{
		return base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::allocate<U>( );
	}

	template<typename U>
	void deallocate( U* p )
	{
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::deallocate<U>( p );
	}
	
	template<>
	T* allocate<T>( )
	{
		return Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::deallocate( p );
	}
	
	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::m_data ) &&
				( object <  Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::m_data + pool_page_size_helper<PageSize>::value * PageMaxCount * type_count<T, TList ...>::value );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return (u32)( ( object - Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::m_data ) / ( pool_page_size_helper<PageSize>::value * PageMaxCount ) );
	}

protected:
	void create( pointer memory )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::create( memory );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::create( memory + pool_page_size_helper<PageSize>::value * PageMaxCount );
	}
};



template<uptr PageSize, typename ... TList>
using poolset = base_poolset<pool, PageSize, TList ...>;

template<uptr PageSize, typename ... TList>
using allocation_poolset = base_poolset<allocation_pool, PageSize, TList ...>;

template<uptr PageSize, uptr PageMaxCount, typename ... TList>
using dynamic_poolset = base_dynamic_poolset<dynamic_pool, PageSize, PageMaxCount, TList ...>;

template<uptr PageSize, uptr PageMaxCount, typename ... TList>
using dynamic_allocation_poolset = base_dynamic_poolset<dynamic_allocation_pool, PageSize, PageMaxCount, TList ...>;

#endif // #ifndef __core_poolset_h_included_