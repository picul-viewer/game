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

render::ui_batch& render::get_ui_batch( )
{
	return g_resources.ui_batch( );
}