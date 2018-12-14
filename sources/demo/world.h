#ifndef __demo_world_h_included_
#define __demo_world_h_included_

#include <types.h>
#include <math/vector.h>

#include <engine/scene.h>
#include <engine/object.h>

#include <system/window_input_state.h>
#include <demo/fly_camera.h>

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

protected:
	math::u32x2 m_dimensions;
	engine::scene* m_scene;
	engine::object m_cubes;
	fly_camera m_camera;

};

extern world g_world;

} // namespace game

#endif // #ifndef __demo_world_h_included_