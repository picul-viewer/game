#include "stage_forward_default.h"

namespace render {

namespace renderer {
	extern renderer_data g_data;
} // namespace renderer

namespace stage_forward_default
{

static void bind_render_target_command( )
{
	render_target_view& backbuffer = renderer::g_data.get_backbuffer( );
	render_target_tex2d& depth_buffer = renderer::g_data.get_depth_buffer( );

	backbuffer.bind( depth_buffer.get_dsv( ) );
}

static void render_mesh_command( render_object_mesh const* current )
{
	current->render( );
}

void execute( )
{
	render_core::get_render_queue( ).push( &bind_render_target_command );

	renderer::g_data.get_rendered_meshes( ).for_each( []( render_object_mesh const** current )
	{
		render_core::get_render_queue( ).push( &render_mesh_command, *current );
	} );
}

} // namespace stage_forward_default

} // namespace render