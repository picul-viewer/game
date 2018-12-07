#include "stage_initialization.h"
#include "renderer.h"
#include "resources.h"
#include "parameters.h"
#include "statistics.h"

namespace render {

void stage_initialization::create( )
{ }

void stage_initialization::destroy( )
{ }

void stage_initialization::execute( )
{
	RENDER_PROFILE_EVENT( stage_initialization );

	RENDER_DEBUG_EVENT( stage_initialization );

	// Initialize renderer.
	g_resources.get_backbuffer( ).clear( math::float4( 0.0f ) );
	g_resources.get_depth_buffer( ).get_dsv( ).clear( 1.0f, 0u );
	
	// Initialize render camera.
	g_renderer.m_render_camera.set_view( g_parameters.camera.view );
	g_renderer.m_render_camera.set_perspective( g_parameters.camera.fov, 1.3333f, 0.01f, 100.0f );
	g_renderer.m_render_camera.update( );

	g_resources.bind_default_resources( );
}

} // namespace render