#include "allocator.h"

#include <Windows.h>

pointer virtual_mem_allocator::reserve( pointer p, uptr size )
{
	return VirtualAlloc( p, size, MEM_RESERVE, PAGE_NOACCESS );
}

pointer virtual_mem_allocator::commit( pointer p, uptr size )
{
	return VirtualAlloc( p, size, MEM_COMMIT, PAGE_READWRITE );
}

void virtual_mem_allocator::decommit( pointer p, uptr size )
{
	VirtualFree( p, size, MEM_DECOMMIT );
}

void virtual_mem_allocator::release( pointer p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}

pointer virtual_mem_allocator::allocate( uptr size )
{
	return VirtualAlloc( nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
}

void virtual_mem_allocator::deallocate( pointer p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}