#include "allocator.h"

#include <Windows.h>

pointer std_allocator::allocate( uptr const size )
{
	return malloc( size );
}

void std_allocator::deallocate( pointer const p )
{
	free( p );
}


std_allocator::scoped_memory::scoped_memory( uptr const size ) :
	m_data( std_allocator( ).allocate( size ) )
{ }

std_allocator::scoped_memory::~scoped_memory( )
{
	std_allocator( ).deallocate( m_data );
}

std_allocator::scoped_memory::operator pointer( ) const
{
	return m_data;
}


pointer virtual_allocator::reserve( pointer const p, uptr const size )
{
	return VirtualAlloc( p, size, MEM_RESERVE, PAGE_NOACCESS );
}

pointer virtual_allocator::commit( pointer const p, uptr const size )
{
	return VirtualAlloc( p, size, MEM_COMMIT, PAGE_READWRITE );
}

void virtual_allocator::decommit( pointer const p, uptr const size )
{
	VirtualFree( p, size, MEM_DECOMMIT );
}

void virtual_allocator::release( pointer const p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}

pointer virtual_allocator::allocate( uptr const size )
{
	return VirtualAlloc( nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
}

void virtual_allocator::deallocate( pointer const p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}


fake_allocator::fake_allocator( pointer const data )
	: m_data( data )
{ }

pointer fake_allocator::allocate( uptr const )
{
	return m_data;
}

void fake_allocator::deallocate( pointer )
{ }