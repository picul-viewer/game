#ifndef GUARD_ENGINE_WORLD_H_INCLUDED
#define GUARD_ENGINE_WORLD_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include <system/interlocked.h>
#include <system/system_event.h>
#include <system/thread.h>
#include <system/window.h>

#include <utils/engine_threads.h>

#include "scene.h"
#include "object.h"

namespace engine {

class world
{
public:
	void run( math::u32x2 const in_window_dimensions );
	void exit( );

	void set_current_scene( scene* const in_scene ) const;
	
	inline math::u32x2 get_window_dimensions( ) const { return m_window_dimensions; }
	inline bool is_active( ) const { return m_is_active; }

private:
	struct helper;

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool const is_active );
	void window_char( u32 const key );
	void window_input( );

	void window_thread( );
	void main_thread( );
	void fs_thread( );
	void helper_thread( );

	void create( );
	void destroy( );

private:
	sys::window m_window;

	sys::thread m_threads[engine_thread_count];
	
	sys::system_event m_alive_events[engine_busy_threads_count];
	sys::system_event m_exit_events[engine_busy_threads_count];

	math::u32x2 m_window_dimensions;
	u32 m_thread_count;
	mt_u32 m_helper_count;
	bool m_is_active;

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

#endif // #ifndef GUARD_ENGINE_WORLD_H_INCLUDED