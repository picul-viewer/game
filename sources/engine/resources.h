#ifndef __engine_resources_h_included_
#define __engine_resources_h_included_

#include <types.h>
#include <lib/pool.h>

#include "scene.h"

namespace engine {

class resources
{
public:
	typedef pool<sizeof(scene), Memory_Page_Size> scene_pool;

public:
	void create( );
	void destroy( );

	inline scene_pool& get_scene_pool( ) { return m_scene_pool; }

protected:
	scene_pool m_scene_pool;

};

extern resources g_resources;

} // namespace engine

#endif // #ifndef __engine_resources_h_included_