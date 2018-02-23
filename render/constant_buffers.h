#ifndef __render_constant_buffers_h_included_
#define __render_constant_buffers_h_included_

#include <types.h>
#include "dx_include.h"

#include "constant_buffer.h"
#include "per_frame_constant_buffer.h"
#include "per_instance_constant_buffer.h"
#include "miscellaneous_constant_buffer.h"

namespace render {

static const u32 c_default_constant_buffers_count = 3;

namespace constant_buffers
{
	void create( );
	void destroy( );

	per_frame_constant_buffer const& get_per_frame_constant_buffer( );
	per_instance_constant_buffer const& get_per_instance_constant_buffer( );
	miscellaneous_constant_buffer const& get_miscellaneous_constant_buffer( );

	void bind_vs( );
	void bind_ps( );
	void bind_gs( );
	void bind_hs( );
	void bind_ds( );
	void bind_cs( );
}

} // namespace render

#endif // #ifndef __render_constant_buffers_h_included_