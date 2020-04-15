#ifndef GUARD_RENDER_OBJECT_INLINE_H_INCLUDED
#define GUARD_RENDER_OBJECT_INLINE_H_INCLUDED

#include "render_object_mesh.h"

namespace render {

template<typename F>
void object::for_each_mesh_object( F const& in_functor ) const
{
	render_object_mesh* mesh_ptr = (render_object_mesh*)m_data;
	render_object_mesh* const last = mesh_ptr + m_mesh_count;

	for ( ; mesh_ptr != last; ++mesh_ptr )
		in_functor( mesh_ptr );
}

} // namespace render

#endif // #ifndef GUARD_RENDER_OBJECT_INLINE_H_INCLUDED