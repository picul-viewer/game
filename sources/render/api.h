#ifndef GUARD_RENDER_API_H_INCLUDED
#define GUARD_RENDER_API_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include "parameters_type.h"
#include "handles.h"

namespace render {

class scene;
class ui_batch;

void create( pvoid const in_hwnd, math::u16x2 const in_resolution, bool const in_is_windowed );
void destroy( );
bool ready( );
void update( );

void copy_thread_job( u32 const in_thread_index, volatile bool& in_alive_flag );

parameters& get_parameters( );

void set_current_scene( scene* in_scene );

ui_batch& get_ui_batch( );

} // namespace render

#endif // #ifndef GUARD_RENDER_API_H_INCLUDED