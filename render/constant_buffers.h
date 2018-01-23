#ifndef __render_constant_buffers_h_included_
#define __render_constant_buffers_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

#include "constant_buffer.h"

#include "per_frame_constants.h"

namespace render {

namespace constant_buffers
{
	enum {
		default_constant_buffers_count		= 2,

		per_frame_constant_buffer_size		= sizeof(per_frame_constants),
		per_instance_constant_buffer_size	= 16 * sizeof(math::float4),
	};

	void create( );
	void destroy( );

	constant_buffer const& get_per_frame_constant_buffer( );
	constant_buffer const& get_per_instance_constant_buffer( );

	void bind_vs( );
	void bind_ps( );
	void bind_gs( );
	void bind_hs( );
	void bind_ds( );
	void bind_cs( );

	void bind( );
}

} // namespace render

#endif // #ifndef __render_constant_buffers_h_included_