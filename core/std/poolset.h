#ifndef __core_poolset_h_included_
#define __core_poolset_h_included_

#include <core/types.h>
#include <core/macros.h>
#include "pool.h"

template<template<uptr, uptr> class Base, uptr PageSize, typename ... TList>
struct base_poolset;

template<template<uptr, uptr> class Base, uptr PageSize, typename T>
struct base_poolset<Base, PageSize, T> : Base<sizeof(T), PageSize / sizeof(T)>
{
public:
	using Base<sizeof(T), PageSize / sizeof(T)>::destroy;

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
};

template<template<uptr, uptr> class Base, uptr PageSize, typename T, typename ... TList>
struct base_poolset<Base, PageSize, T, TList ...> : base_poolset<Base, PageSize, TList ...>, Base<sizeof(T), PageSize / sizeof(T)>
{
public:
	void destroy( )
	{
		Base<sizeof(T), PageSize / sizeof(T)>::destroy( );
		base_poolset<PageSize, TList ...>::destroy( );
	}
	
	template<typename U>
	U* allocate( )
	{
		return base_poolset<PageSize, TList ...>::allocate<U>( );
	}

	template<typename U>
	void deallocate( U* p )
	{
		base_poolset<PageSize, TList ...>::deallocate<U>( p );
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
};


template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename ... TList>
struct base_dynamic_poolset;

template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename T>
struct base_dynamic_poolset<Base, PageSize, PageMaxCount, T> : Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>
{
public:
	using Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::destroy;

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
};

template<template<uptr, uptr, uptr> class Base, uptr PageSize, uptr PageMaxCount, typename T, typename ... TList>
struct base_dynamic_poolset<Base, PageSize, PageMaxCount, T, TList ...> :
	base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>,
	Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>
{
public:
	void destroy( )
	{
		Base<sizeof(T), PageSize / sizeof(T), PageMaxCount>::destroy( );
		base_dynamic_poolset<Base, PageSize, PageMaxCount, TList ...>::destroy( );
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