#ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_INLINE_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_INLINE_H_INCLUDED

namespace render {

template<typename Callback>
void render_object_allocator::for_each( render_object* const in_objects, Callback const& in_callback )
{
	render_object* obj = in_objects;

	while ( obj != nullptr )
	{
		ASSERT( m_mesh_allocator.contains( obj ) );

		render_object* const next = obj->m_next;

		render_object_mesh* const arg = (render_object_mesh*)obj;
		in_callback( arg );

		obj = next;
	}
}

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_INLINE_H_INCLUDED