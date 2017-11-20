#ifndef __game_game_world_h_included_
#define __game_game_world_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/structs.h>
#include <core/game.h>

#include "game_object.h"

namespace game {

class world
{
public:
	void create( );
	void destroy( );

	void load( config& in_config );

protected:
	typedef embedded_list<object, &object::m_next> objects_list;
	objects_list	m_objects;
};

} // namespace game

#endif // #ifndef __game_game_world_h_included_