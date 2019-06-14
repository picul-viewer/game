#ifndef GUARD_DEMO_WORLD_H_INCLUDED
#define GUARD_DEMO_WORLD_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include <engine/scene.h>
#include <engine/object.h>

#include <system/time.h>
#include <system/window_input_state.h>

#include <ui/console.h>

#include <utils/fly_camera.h>

namespace game {

class world
{
public:
	void create( );
	void update( );
	void destroy( );

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool const is_active );
	void window_char( u32 const key );
	void window_input( );

private:
	void initialize_console( );
	static void on_console_command( pcstr const str );

private:
	engine::scene* m_scene;
	engine::object m_cubes;
	utils::fly_camera m_camera;
	ui::console m_console;
	sys::ticker m_ticker;
	bool m_console_visible;

};

extern world g_world;

} // namespace game

#endif // #ifndef GUARD_DEMO_WORLD_H_INCLUDED