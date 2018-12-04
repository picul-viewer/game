#ifndef __render_world_h_included_
#define __render_world_h_included_

#include <types.h>
#include <math/vector.h>

#include "parameters.h"

namespace render {

class scene;
class object;
class texture;
class ui_batch;

class world
{
public:
	void create( pvoid in_hwnd, math::u16x2 in_resolution, bool in_is_windowed );
	void update( );
	void destroy( );

	parameters& get_parameters( ) const;

	scene* create_scene( ) const;
	void set_current_scene( scene* in_scene ) const;
	void destroy_scene( scene* in_scene ) const;

	object* create_object( ) const;
	void destroy_object( object* in_object ) const;

	texture* create_texture( pcstr in_path ) const;
	void destroy_texture( texture* in_texture ) const;

	ui_batch& get_ui_batch( ) const;

};

extern world g_world;

} // namespace render

#endif // #ifndef __render_world_h_included_