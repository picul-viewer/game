#ifndef __render_render_core_h_included_
#define __render_render_core_h_included_

#include <types.h>
#include <core/math.h>
#include "dx_include.h"
#include <core/threads.h>

namespace render {

namespace core
{
	typedef task_queue<64> render_context_queue;
	typedef task_queue<64> render_device_queue;

	void create( HWND in_hwnd, math::u16x2 in_resolution, bool in_is_windowed, bool in_allow_debug );
	void destroy( );
	void wait_for_destruction( );
	
	void suspend( );
	void resume( );

	void end_frame( );

	u32 render_frame_id( );
	u32 context_frame_id( );

	render_context_queue&	get_context_queue( );
	render_device_queue&	get_device_queue( );

} // namespace core

} // namespace render

#endif // #ifndef __render_render_core_h_included_