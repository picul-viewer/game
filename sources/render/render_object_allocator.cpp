#include "render_object_allocator.h"

namespace render {

void render_object_allocator::create( )
{
	uptr const mesh_memory_size = mesh_max_count * sizeof(render_object_mesh);

	uptr const memory_size =
		mesh_memory_size;

	pointer const memory = virtual_allocator( ).allocate( memory_size );

	pointer ptr = memory;

	m_mesh_allocator.create( ptr, mesh_memory_size );
	ptr += mesh_memory_size;

	ASSERT_CMP( ptr, ==, memory + memory_size );
}

void render_object_allocator::destroy( )
{
	pointer const memory = m_mesh_allocator.data( );

	virtual_allocator( ).deallocate( memory );
}

} // namespace render