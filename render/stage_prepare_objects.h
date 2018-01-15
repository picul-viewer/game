#ifndef __render_stage_prepare_objects_h_included_
#define __render_stage_prepare_objects_h_included_

#include <core/types.h>

#include "render_scene.h"
#include "renderer.h"

namespace render {

namespace stage_prepare_objects
{
	void execute( );

	void dispatch_object_mesh( render_object_mesh const* in_object );

} // namespace stage_prepare_objects

} // namespace render

#endif // #ifndef __render_stage_prepare_objects_h_included_