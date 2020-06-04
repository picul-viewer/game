#ifndef GUARD_RENDER_API_H_INCLUDED
#define GUARD_RENDER_API_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include "parameters_type.h"
#include "handles.h"

namespace render {

class scene;

void create( pvoid const in_hwnd, math::u16x2 const in_resolution, bool const in_is_windowed );
void destroy( );
bool ready( );
void update( );

void copy_thread_job( u32 const in_thread_index, volatile bool& in_alive_flag );

parameters& get_parameters( );

void set_current_scene( scene* in_scene );

void ui_add_quad(
	math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
	math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture );
void ui_add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color );
void ui_next_level( );

void reload( );

} // namespace render

#endif // #ifndef GUARD_RENDER_API_H_INCLUDED