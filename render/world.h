#ifndef __render_world_h_included_
#define __render_world_h_included_

#include <types.h>
#include "dx_include.h"
#include <math/vector.h>

#include "render_parameters.h"

namespace render {

class scene;

class world
{
public:
	void run( pvoid in_hwnd, math::u16x2 in_resolution, bool in_is_windowed, bool in_allow_debug );
	void exit( );

	parameters& get_parameters( ) const;

	scene* create_scene( ) const;
	void set_current_scene( scene* in_scene ) const;
	void destroy_scene( scene* in_scene ) const;

protected:
	volatile bool m_alive;

};

extern world g_world;

} // namespace render

#endif // #ifndef __render_render_core_h_included_