#ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/pool_allocator.h>

#include "render_object_mesh.h"

namespace render {

class render_object_allocator
{
public:
	enum {
		mesh_max_count = 16384
	};

	typedef lib::pool_allocator<sizeof(render_object_mesh)> mesh_allocator_type;

public:
	void create( );
	void destroy( );

	inline mesh_allocator_type& mesh_allocator( ) { return m_mesh_allocator; }

	template<typename Callback>
	void for_each( render_object* const in_objects, Callback const& in_callback );

private:
	mesh_allocator_type m_mesh_allocator;

};

} // namespace render

#include "render_object_allocator_inline.h"

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_H_INCLUDED