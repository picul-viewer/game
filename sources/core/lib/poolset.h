#ifndef GUARD_CORE_POOLSET_H_INCLUDED
#define GUARD_CORE_POOLSET_H_INCLUDED

#include <types.h>
#include <macros.h>
#include "pool.h"

template<template<uptr, uptr> class Base, uptr PageSize, typename ... TList>
struct base_poolset;

template<template<uptr, uptr> class Base, uptr PageSize, typename T>
struct base_poolset<Base, PageSize, T> : Base<sizeof(T), PageSize>
{
public:
	using Base<sizeof(T), PageSize>::create;
	using Base<sizeof(T), PageSize>::destroy;
	using Base<sizeof(T), PageSize>::clear;

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
		return Base<sizeof(T), PageSize>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize>::deallocate( p );
	}

	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize>::m_data ) &&
				( object <  Base<sizeof(T), PageSize>::m_data + virtual_allocator::memory_size_helper<PageSize>::value );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return 0;
	}
};

template<template<uptr, uptr> class Base, uptr PageSize, typename T, typename ... TList>
struct base_poolset<Base, PageSize, T, TList ...> :
	Base<sizeof(T), PageSize>,
	base_poolset<Base, PageSize, TList ...>
{
public:
	void create( )
	{
		Base<sizeof(T), PageSize>::create( virtual_allocator( ).commit( nullptr, virtual_allocator::memory_size_helper<PageSize>::value * type_count<T, TList ...>::value ) );
		base_poolset<Base, PageSize, TList ...>::create( Base<sizeof(T), PageSize>::m_data + virtual_allocator::memory_size_helper<PageSize>::value );
	}

	void destroy( )
	{
		Base<sizeof(T), PageSize>::destroy( );
		base_poolset<Base, PageSize, TList ...>::destroy( );
	}
	
	void clear( )
	{
		Base<sizeof(T), PageSize>::clear( );
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
		return Base<sizeof(T), PageSize>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize>::deallocate( p );
	}
	
	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize>::m_data ) &&
				( object <  Base<sizeof(T), PageSize>::m_data + virtual_allocator::memory_size_helper<PageSize>::value * type_count<T, TList ...>::value );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return (u32)( ( object - Base<sizeof(T), PageSize>::m_data ) / virtual_allocator::memory_size_helper<PageSize>::value );
	}

protected:
	void create( pointer memory )
	{
		Base<sizeof(T), PageSize>::create( memory );
		base_poolset<Base, PageSize, TList ...>::create( memory + virtual_allocator::memory_size_helper<PageSize>::value );
	}
};


template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename ... TList>
struct base_dynamic_poolset;

template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename T>
struct base_dynamic_poolset<Base, PageSize, PageMaxCount, T> : Base<sizeof(T), PageSize, PageMaxCount>
{
public:
	using Base<sizeof(T), PageSize, PageMaxCount>::create;
	using Base<sizeof(T), PageSize, PageMaxCount>::destroy;
	using Base<sizeof(T), PageSize, PageMaxCount>::clear;

	template<typename U>
	U* allocate( )
	{
		static_assert( false, "unsupported type" );
	}

	template<typename U>
	void deallocate( U* p )
	{
		static_assert( false, "unsupported type" );
	}
	
	template<>
	T* allocate<T>( )
	{
		return Base<sizeof(T), PageSize, PageMaxCount>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize, PageMaxCount>::deallocate( p );
	}
	
	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize, PageMaxCount>::m_data ) &&
				( object <  Base<sizeof(T), PageSize, PageMaxCount>::m_data + virtual_allocator::memory_size_helper<PageSize>::value * PageMaxCount );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return 0;
	}
};

template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename T, typename ... TList>
struct base_dynamic_poolset<Base, PageSize, PageMaxCount, T, TList ...> :
	Base<sizeof(T), PageSize, PageMaxCount>,
	base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>
{
public:
	void create( )
	{
		Base<sizeof(T), PageSize, PageMaxCount>::create( virtual_allocator( ).reserve( nullptr, virtual_allocator::memory_size_helper<PageSize>::value * PageMaxCount * type_count<T, TList ...>::value ) );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::create( Base<sizeof(T), PageSize, PageMaxCount>::m_data + virtual_allocator::memory_size_helper<PageSize>::value * PageMaxCount );
	}

	void destroy( )
	{
		Base<sizeof(T), PageSize, PageMaxCount>::destroy( );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::destroy( );
	}
	
	void clear( )
	{
		Base<sizeof(T), PageSize, PageMaxCount>::clear( );
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
		return Base<sizeof(T), PageSize, PageMaxCount>::allocate( sizeof(T) );
	}

	template<>
	void deallocate<T>( T* p )
	{
		Base<sizeof(T), PageSize, PageMaxCount>::deallocate( p );
	}
	
	u32 get_object_type( pointer object )
	{
#ifdef DEBUG
		auto f = [&]( )
		{
			return
				( object >= Base<sizeof(T), PageSize, PageMaxCount>::m_data ) &&
				( object <  Base<sizeof(T), PageSize, PageMaxCount>::m_data + virtual_allocator::memory_size_helper<PageSize>::value * PageMaxCount * type_count<T, TList ...>::value );
		};

		ASSERT( f( ) );
#endif // #ifdef DEBUG
		return (u32)( ( object - Base<sizeof(T), PageSize, PageMaxCount>::m_data ) / ( virtual_allocator::memory_size_helper<PageSize>::value * PageMaxCount ) );
	}

protected:
	void create( pointer memory )
	{
		Base<sizeof(T), PageSize, PageMaxCount>::create( memory );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::create( memory + virtual_allocator::memory_size_helper<PageSize>::value * PageMaxCount );
	}
};



template<uptr PageSize, typename ... TList>
using poolset = base_poolset<pool, PageSize, TList ...>;

template<uptr PageSize, uptr PageMaxCount, typename ... TList>
using dynamic_poolset = base_dynamic_poolset<dynamic_pool, PageSize, PageMaxCount, TList ...>;

#endif // #ifndef GUARD_CORE_POOLSET_H_INCLUDED