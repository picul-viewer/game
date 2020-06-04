#include "api.h"

#include <macros.h>
#include "gpu_uploader.h"
#include "parameters_manager.h"
#include "resources.h"
#include "render.h"

void render::create( pvoid const in_hwnd, math::u16x2 const in_resolution, bool const in_is_windowed )
{
	parameters& params = get_parameters( );

	params.hwnd = in_hwnd;
	params.screen_resolution = in_resolution;
	params.is_windowed = in_is_windowed;
	params.draw_statistics = false;

	g_parameters_manager.update( );

	g_render.create( );
}

void render::destroy( )
{
	g_render.destroy( );
}

bool render::ready( )
{
	return g_render.ready( );
}

void render::update( )
{
	g_render.update( );
}

void render::copy_thread_job( u32 const in_thread_index, volatile bool& in_alive_flag )
{
	g_gpu_uploader.copy_thread_job( in_thread_index, in_alive_flag );
}

render::parameters& render::get_parameters( )
{
	return g_parameters_manager.get_parameters_for_modification( );
}

void render::set_current_scene( scene* in_scene )
{
	g_render.set_scene( in_scene );
}

void render::ui_add_quad(
	math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
	math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture )
{
	g_render.ui_add_quad( in_corners_position, in_corners_texcoord, in_mult_color, in_add_color, in_texture );
}

void render::ui_add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color )
{
	g_render.ui_add_color_quad( in_corners_position, in_color );
}

void render::ui_next_level( )
{
	g_render.ui_next_level( );
}

void render::reload( )
{
	g_render.reload( );
}