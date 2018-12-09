#include "allocator.h"

#include <Windows.h>

pointer std_allocator::allocate( uptr size )
{
	return malloc( size );
}

void std_allocator::deallocate( pointer p )
{
	free( p );
}


pointer virtual_allocator::reserve( pointer p, uptr size )
{
	return VirtualAlloc( p, size, MEM_RESERVE, PAGE_NOACCESS );
}

pointer virtual_allocator::commit( pointer p, uptr size )
{
	return VirtualAlloc( p, size, MEM_COMMIT, PAGE_READWRITE );
}

void virtual_allocator::decommit( pointer p, uptr size )
{
	VirtualFree( p, size, MEM_DECOMMIT );
}

void virtual_allocator::release( pointer p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}

pointer virtual_allocator::allocate( uptr size )
{
	return VirtualAlloc( nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
}

void virtual_allocator::deallocate( pointer p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}


fake_allocator::fake_allocator( pointer data )
	: m_data( data )
{ }

pointer fake_allocator::allocate( uptr )
{
	return m_data;
}

void fake_allocator::deallocate( pointer )
{ }