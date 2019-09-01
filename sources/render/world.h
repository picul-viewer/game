#ifndef GUARD_RENDER_WORLD_H_INCLUDED
#define GUARD_RENDER_WORLD_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include "parameters_type.h"
#include "handles.h"

namespace render {

class scene;
class object;
class ui_batch;

class world
{
public:
	void create( pvoid in_hwnd, math::u16x2 in_resolution, bool in_is_windowed );
	void update( );
	void destroy( );

	parameters& get_parameters( ) const;

	void set_current_scene( scene* in_scene ) const;

	ui_batch& get_ui_batch( ) const;

};

extern world g_world;

} // namespace render

#endif // #ifndef GUARD_RENDER_WORLD_H_INCLUDED