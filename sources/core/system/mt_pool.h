#ifndef GUARD_CORE_MT_POOL_H_INCLUDED
#define GUARD_CORE_MT_POOL_H_INCLUDED

#include <types.h>
#include "interlocked.h"

namespace sys {

template<typename T, u32 MemorySize>
class mt_pool
{
public:
	void create( pointer const allocated_memory = nullptr );
	void destroy( );

	T* allocate( );
	void deallocate( T* const p );

	T& operator[]( u32 const index ) const;
	u32 index_of( T* const p ) const;

	bool contains_pointer( pointer const p ) const;

private:
	enum : u32 { null_value = 0xFFFFFFFF };

private:
	mem_align(Cache_Line)
	mt_u32 m_last_pointer;
	mem_align(Cache_Line)
	mt_u32 m_push_pointer;
	mem_align(Cache_Line)
	T* m_data;

};

template<typename T, u32 MemorySize, u32 PageSize>
class mt_dynamic_pool
{
public:
	void create( pointer const reserved_memory = nullptr );
	void destroy( );

	T* allocate( );
	void deallocate( T* const p );

	T& operator[]( u32 const index ) const;
	u32 index_of( T* const p ) const;

	bool contains_pointer( pointer const p ) const;

private:
	enum : u32 { null_value = 0xFFFFFFFF };

private:
	mem_align(Cache_Line)
	mt_u32 m_last_pointer;
	mem_align(Cache_Line)
	mt_u32 m_push_pointer;
	mem_align(Cache_Line)
	mt_u32 m_page_pointer;
	mem_align(Cache_Line)
	T* m_data;

};

} // namespace sys

#include "mt_pool_inline.h"

#endif // #ifndef GUARD_CORE_MT_POOL_H_INCLUDED