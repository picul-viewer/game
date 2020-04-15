#ifndef GUARD_RENDER_RENDER_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_RENDER_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/heap.h>
#include <system/mutex.h>

namespace render {

class render_allocator
{
public:
	void create( uptr const in_size );
	void destroy( );

	pointer allocate( uptr const in_size );
	void deallocate( pointer const in_ptr );

	u32 offset( pointer const in_ptr ) const;
	pointer offset_to_ptr( u32 const in_offset ) const;

private:
	heap m_allocator;
	sys::mutex m_mutex;
	pointer m_allocator_memory;
	uptr m_allocator_size;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_ALLOCATOR_H_INCLUDED