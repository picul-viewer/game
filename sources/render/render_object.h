#ifndef GUARD_RENDER_RENDER_OBJECT_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_H_INCLUDED

#include <types.h>

namespace render {

enum render_object_type
{
	render_object_type_mesh = 0,

	// ...

	render_object_type_count
};

struct render_object
{
	render_object* m_next;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_H_INCLUDED