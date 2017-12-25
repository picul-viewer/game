#ifndef __render_render_allocators_h_included_
#define __render_render_allocators_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/std.h>

#include "render_object_mesh.h"

namespace render {

class render_objects_allocator
{
public:
	template<typename T>
	T* allocate( );
	template<typename T>
	void deallocate( T* in_object );

	template<typename Callback, typename ... Args>
	void execute_typed( render_object* in_object, Callback const& in_functor, Args ... in_args );

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

#include "render_allocators_inline.h"

#endif // #ifndef __render_render_allocators_h_included_