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
	void run( math::u32x2 in_window_dimensions );
	void exit( );

	scene* create_scene( ) const;
	void set_current_scene( scene* in_scene ) const;
	void destroy_scene( scene* in_scene ) const;
	
private:
	struct helper;

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool const is_active );
	void window_char( u32 const key );
	void window_input( );

	void window_thread( );
	void main_thread( );

	void create( );
	void destroy( );

private:
	enum threads
	{
		thread_window = 0,

		thread_count
	};

	enum events
	{
		event_window = 0,

		event_main,
		
		event_count
	};

	enum {
		system_event_count = event_count - 1
	};

private:
	sys::window m_window;

	thread m_threads[thread_count];
	
	threading_event m_alive_events[event_count];
	threading_event m_exit_events[event_count];

	math::u32x2 m_window_dimensions;
	bool m_is_active;

	volatile u32 m_systems_alive;
	volatile u32 m_systems_running;
	volatile bool m_alive;

};

extern world g_world;

} // namespace engine

/*	World interface is the interface for communication with game-logic project.
	This defined as namespace because it's stateless. Actual state supposed to
	be encapsulated in "game::world" class, and this interface's functions are
	supposed to be implemented as calls to corresponding methods of this class. */
namespace game {

namespace world_interface {

void create( );
void update( );
void destroy( );

void window_resize( math::u32x2 const& new_dimensions );
void window_activate( bool const is_active );
void window_char( u32 const key );
void window_input( );

} // namespace world_interface

} // namespace game

#endif // #ifndef __engine_world_h_included_