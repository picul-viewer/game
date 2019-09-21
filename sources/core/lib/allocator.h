#ifndef GUARD_CORE_ALLOCATOR_H_INCLUDED
#define GUARD_CORE_ALLOCATOR_H_INCLUDED

#include <types.h>

struct std_allocator
{
	pointer allocate( uptr const size );
	void deallocate( pointer const p );

	struct scoped_memory
	{
	public:
		scoped_memory( uptr const size );
		~scoped_memory( );

		operator pointer( ) const;

		template<typename T>
		operator T*( ) const
		{
			return (T*)m_data;
		}

	private:
		pvoid m_data;

	};
};

template<uptr Alignment>
struct aligned_std_allocator
{
	inline pointer allocate( uptr const size );
	inline void deallocate( pointer const p );
};

static inline uptr get_aligned_size( uptr const size, uptr const page_size )
{
	return ( size + ( page_size - 1 ) ) & ( ~( page_size - 1 ) );
}

struct virtual_allocator
{
	template<uptr MemorySize>
	struct memory_size_helper
	{
		static const uptr value = ( MemorySize + ( Memory_Page_Size - 1 ) ) & ( ~( Memory_Page_Size - 1 ) );
	};

	pointer reserve( pointer const p, uptr const size );
	pointer commit( pointer const p, uptr const size );
	void decommit( pointer const p, uptr const size );
	void release( pointer const p );

	pointer allocate( uptr const size );
	void deallocate( pointer const p );
};

#define stack_allocate( n ) (pointer)_alloca( n )

struct fake_allocator
{
	fake_allocator( pointer const data );

	pointer allocate( uptr const size );
	void deallocate( pointer const p );

private:
	pointer m_data;

};

#include "allocator_inline.h"

#endif // #ifndef GUARD_CORE_ALLOCATOR_H_INCLUDED
