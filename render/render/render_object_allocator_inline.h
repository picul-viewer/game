#ifndef __render_render_object_allocator_inline_h_included_
#define __render_render_object_allocator_inline_h_included_

namespace render {

template<typename T>
T* render_objects_allocator::allocate( )
{
	return m_allocator.allocate<T>( );
}

template<typename T>
void render_objects_allocator::deallocate( T* in_object )
{
	m_allocator.deallocate( in_object );
}

template<typename Callback, typename ... Args>
void render_objects_allocator::execute_typed( render_object* in_object, Callback in_functor, Args ... in_args )
{
	switch ( m_allocator.get_object_type( in_object ) )
	{
	case 0:
	{
		// mesh
		render_object_mesh* obj = (render_object_mesh*)in_object;
		in_functor.call<render_object_mesh>( obj, in_args ... );
		break;
	}
	default:
		UNREACHABLE_CODE
	}
}

} // namespace render

#endif // #ifndef __render_render_object_allocator_inline_h_included_