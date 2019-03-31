#ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_INLINE_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_INLINE_H_INCLUDED

namespace render {

template<typename T>
T* render_object_allocator::allocate( )
{
	return m_allocator.allocate<T>( );
}

template<typename T>
void render_object_allocator::deallocate( T* in_object )
{
	m_allocator.deallocate( in_object );
}

template<typename Callback, typename ... Args>
void render_object_allocator::execute( render_object* in_object, Callback const& in_functor, Args ... in_args )
{
	switch ( m_allocator.get_object_type( in_object ) )
	{
	case 0:
	{
		// mesh
		render_object_mesh* obj = (render_object_mesh*)in_object;
		in_functor( obj, in_args ... );
		break;
	}
	default:
		UNREACHABLE_CODE
	}
}

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_INLINE_H_INCLUDED