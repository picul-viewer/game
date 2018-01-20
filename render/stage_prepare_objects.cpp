#include "stage_prepare_objects.h"

namespace render {

namespace renderer {
	extern renderer_data	g_data;
	extern scene*			g_scene;
} // namespace renderer

namespace stage_prepare_objects
{

void execute( )
{
	renderer::g_scene->dispatch( );
}

void dispatch_object_mesh( render_object_mesh const* in_object )
{
	renderer::g_data.get_rendered_meshes( ).push_back( in_object );
}

} // namespace stage_prepare_objects

} // namespace render