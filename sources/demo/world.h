#ifndef GUARD_DEMO_WORLD_H_INCLUDED
#define GUARD_DEMO_WORLD_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include <engine/scene.h>
#include <engine/object.h>

#include <resource_system/queried_resources.h>

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
	bool ready( );
	void destroy( );

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool const is_active );
	void window_char( u32 const key );
	void window_input( );

private:
	void initialize_console( );
	static void on_console_command( pcstr const str );

	void on_resources_ready( queried_resources& resources );

	void on_resources_destroyed( );

private:
	engine::scene::ptr m_scene;
	engine::object::ptr m_object;
	utils::fly_camera m_camera;
	ui::console m_console;
	sys::ticker m_ticker;
	ui::font::ptr m_console_font;
	double m_angle;
	bool m_console_visible;
	volatile bool m_ready;

};

extern world g_world;

} // namespace game

#endif // #ifndef GUARD_DEMO_WORLD_H_INCLUDED