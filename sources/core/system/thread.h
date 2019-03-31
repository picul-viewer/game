#ifndef GUARD_CORE_THREAD_H_INCLUDED
#define GUARD_CORE_THREAD_H_INCLUDED

#include <types.h>

class thread
{
public:
	static const u32 max_priority = 30;

public:
	thread( ) = default;
	~thread( ) = default;

	typedef unsigned long ( __stdcall *thread_func_type )( void* );

	thread( thread_func_type const func, u32 const stack_size, pvoid const arg );
	
	void create( thread_func_type const func, u32 const stack_size, pvoid const arg );
	void destroy( u32 const wait_ms = time::infinite );
	void destroy( u32 const wait_ms, u32 const count );
	void destroy_any( u32 const wait_ms, u32 const count );

	void suspend( );
	void resume( );
	
	void force_processor_index( u32 const index );
	void advise_processor_index( u32 const index );
	void set_priority( u32 const priority );
	void set_affinity_mask( u64 const mask );

	template<void( *Functor )( void* )>
	static unsigned long __stdcall func_helper( void* const arg )
	{
		Functor( arg );
		return 0;
	}
	
	template<void( *Functor )( )>
	static unsigned long __stdcall func_helper( void* const )
	{
		Functor( );
		return 0;
	}

	template<typename ThisType, void( ThisType::*Functor )( )>
	static unsigned long __stdcall method_helper( void* const arg )
	{
		ThisType* this_ptr = (ThisType*)arg;
		( this_ptr->*Functor )( );
		return 0;
	}

protected:
	pvoid	m_id;
};

#endif // #ifndef GUARD_CORE_THREAD_H_INCLUDED
