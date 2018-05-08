#ifndef __render_render_object_h_included_
#define __render_render_object_h_included_

#include <types.h>
#include "dx_include.h"

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

#endif // #ifndef __render_render_object_h_included_