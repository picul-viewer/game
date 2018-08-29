#ifndef __engine_world_h_included_
#define __engine_world_h_included_

#include <types.h>
#include <math/vector.h>

#include <system/window.h>

#include <system/thread.h>
#include <system/threading_event.h>

#include "scene.h"
#include "object.h"

namespace engine {

class world
{
public:
	world( );

	void run( math::u32x2 in_window_dimensions );
	void exit( );

	scene* create_scene( ) const;
	void set_current_scene( scene* in_scene ) const;
	void destroy_scene( scene* in_scene ) const;
	
protected:
	struct helper;

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool is_active );
	void window_input( pvoid handle );
	
	void render_thread( );
	void window_thread( );
	void game_thread( );

	void initialize( );
	void deinitialize( );

protected:
	enum threads
	{
		thread_window = 0,
		thread_game,

		thread_count
	};

protected:
	sys::window m_window;

	thread m_threads[thread_count];

	math::u32x2 m_window_dimensions;
	bool m_is_active;

	volatile bool m_window_created;

};

extern world g_world;

} // namespace engine

/*	World interface is the interface for communication with game-logic project.
	This defined as namespace because it's stateless. Actual state supposed to
	be encapsulated in "game::world" class, and this interface's functions are
	supposed to be implemented as calls to corresponding methods of this class. */
namespace game {

namespace world_interface {

void run( );
void exit( );

void window_resize( math::u32x2 const& new_dimensions );
void window_activate( bool is_active );
void window_input( pvoid handle );

} // namespace world_interface

} // namespace game

#endif // #ifndef __engine_world_h_included_