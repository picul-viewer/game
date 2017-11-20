#ifndef __render_game_object_h_included_
#define __render_game_object_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/structs.h>
#include <core/game.h>
#include "dx_include.h"

#include "render_object.h"

namespace render {

class game_object
{
public:
	void create( config& in_config );
	void destroy( );
	
	void dispatch( ) const;
	
protected:
	typedef embedded_list<render_object, &render_object::m_next> render_object_list;
	render_object_list	m_objects;
	//skeleton*			m_skeleton;
};

} // namespace render

#endif // #ifndef __render_game_object_h_included_