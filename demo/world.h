#ifndef __demo_world_h_included_
#define __demo_world_h_included_

#include <types.h>
#include <math/vector.h>

namespace game {

class world
{
public:
	void run( );
	void exit( );

	void window_resize( math::u32x2 const& new_dimensions );
	void window_activate( bool is_active );
	void window_input( pvoid handle );

protected:
	volatile bool m_alive;

};

extern world g_world;

} // namespace game

#endif // #ifndef __demo_world_h_included_