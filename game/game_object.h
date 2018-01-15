#ifndef __game_game_object_h_included_
#define __game_game_object_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/game.h>

#include <render/game_object.h>

namespace game {

class object
{
public:
	object( );

	void create( config& in_config );
	void destroy( );

protected:
	// Aligned by 64 bytes
	math::float4x4			m_transform;

	render::game_object*	m_render;

	//physics::game_object	m_physic;
	// Or something to handle physics...
	
	// Something to handle sound...

public:
	object*					m_next;
};

} // namespace game

#endif // #ifndef __game_game_object_h_included_