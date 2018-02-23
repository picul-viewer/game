#ifndef __game_game_world_h_included_
#define __game_game_world_h_included_

#include <types.h>
#include <lib/embedded_list.h>
#include <game/config.h>

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