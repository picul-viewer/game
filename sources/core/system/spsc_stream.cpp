#include "spsc_stream.h"
#include <macros.h>
#include <lib/memory.h>

sys::spsc_stream::spsc_stream( pointer const memory, uptr const size )
{
	create( memory, size );
}

void sys::spsc_stream::create( pointer const memory, uptr const size )
{
	ASSERT( memory );
	ASSERT_CMP( size, <, 0x100000000 );

	m_data = memory;
	m_size = (u32)size;

	clear( );
}

void sys::spsc_stream::clear( )
{
	m_push_pointer = 0;
	m_pop_pointer = 0;
}

pvoid sys::spsc_stream::data( ) const
{
	return m_data;
}

pvoid sys::spsc_stream::read_data( u32 const size )
{
	u32 const pop_pointer = m_pop_pointer;
	u32 const new_pop_pointer = pop_pointer + size;

	SPIN_LOCK( new_pop_pointer > m_push_pointer );
	
	pvoid const result = m_data + pop_pointer;
	m_pop_pointer = new_pop_pointer;

	return result;
}

void sys::spsc_stream::write_data( pcvoid const data, u32 const size )
{
	u32 const push_pointer = m_push_pointer;
	u32 const new_push_pointer = push_pointer + size;
	pvoid const destination = m_data + push_pointer;

	ASSERT_CMP( new_push_pointer, <, m_size );
	memory::copy( destination, data, size );

	// First copy all data, and then update pointer.
	store_fence( );

	m_push_pointer = new_push_pointer;
}