#include "render_allocator.h"
#include <macros.h>
#include <lib/allocator.h>

namespace render {

void render_allocator::create( uptr const in_size )
{
	m_allocator_size = in_size;

	m_allocator_memory = virtual_allocator( ).allocate( in_size );
	m_allocator.create( m_allocator_memory, in_size );

	m_mutex.create( );
}

void render_allocator::destroy( )
{
	m_mutex.destroy( );
	m_allocator.destroy( );
	virtual_allocator( ).deallocate( m_allocator_memory );
}

pointer render_allocator::allocate( uptr const in_size )
{
	m_mutex.wait( );
	pointer const result = m_allocator.allocate( in_size );
	m_mutex.release( );
	return result;
}

void render_allocator::deallocate( pointer const in_ptr )
{
	m_mutex.wait( );
	m_allocator.deallocate( in_ptr );
	m_mutex.release( );
}

u32 render_allocator::offset( pointer const in_ptr ) const
{
	uptr const offset = (uptr)in_ptr - (uptr)m_allocator_memory;
	ASSERT_CMP( offset, <, m_allocator_size );

	return (u32)offset;
}

pointer render_allocator::offset_to_ptr( u32 const in_offset ) const
{
	ASSERT_CMP( in_offset, <, m_allocator_size );
	return m_allocator_memory + in_offset;
}

} // namespace render