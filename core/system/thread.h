#ifndef __core_thread_h_included_
#define __core_thread_h_included_

#include <types.h>

class thread
{
public:
	thread( ) = default;
	~thread( ) = default;

	typedef unsigned long ( __stdcall *thread_func_type )( void* );

	thread( thread_func_type func, u32 stack_size, pvoid arg );
	
	void create( thread_func_type func, u32 stack_size, pvoid arg );
	void destroy( u32 wait_ms = 0 );
	void destroy( u32 wait_ms, u32 count );
	void destroy_any( u32 wait_ms, u32 count );

	void suspend( );
	void resume( );

	template<void( *Functor )( void* )>
	static unsigned long __stdcall func_helper( void* arg )
	{
		Functor( arg );
		return 0;
	}

	template<typename ThisType, void( ThisType::*Functor )( )>
	static unsigned long __stdcall method_helper( void* arg )
	{
		ThisType* this_ptr = (ThisType*)arg;
		( this_ptr->*Functor )( );
		return 0;
	}

protected:
	pvoid	m_id;
};

#endif // #ifndef __core_thread_h_included_
