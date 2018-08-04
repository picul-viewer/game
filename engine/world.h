#ifndef __engine_world_h_included_
#define __engine_world_h_included_

#include <types.h>
#include <math/vector.h>
#include <math/camera.h>

#include <system/window.h>

#include <system/thread.h>
#include <system/threading_event.h>

#include "scene.h"
#include "object.h"

namespace engine {

class world
{
public:
	void run( math::u32x2 in_window_dimensions );
	void exit( );

	scene* create_scene( ) const;
	void set_current_scene( scene* in_scene ) const;
	void destroy_scene( scene* in_scene ) const;
	
	void set_current_camera( camera* in_camera ) const;

protected:
	struct helper;

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool is_active );

	void window_thread( );

	void initialize( );
	void deinitialize( );

protected:
	sys::window m_window;

	thread m_window_thread;

	math::u32x2 m_window_dimensions;
	bool m_is_active;

	volatile bool m_window_created;
	volatile bool m_alive;

};

extern world g_world;

} // namespace engine

#endif // #ifndef __engine_world_h_included_