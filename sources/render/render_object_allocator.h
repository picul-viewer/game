#ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/poolset.h>

#include "render_object_mesh.h"

namespace render {

class render_object_allocator
{
public:
	void create( );
	void destroy( );

	template<typename T>
	T* allocate( );
	template<typename T>
	void deallocate( T* in_object );

	template<typename Callback, typename ... Args>
	void execute( render_object* in_object, Callback const& in_functor, Args ... in_args );

protected:
	enum {
		memory_pages_per_render_object = 256
	};

	typedef dynamic_poolset<Memory_Page_Size, memory_pages_per_render_object,
		// Here go all types of objects
		render_object_mesh
	> allocator_type;

	allocator_type m_allocator;
};

} // namespace render

#include "render_object_allocator_inline.h"

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_ALLOCATOR_H_INCLUDED