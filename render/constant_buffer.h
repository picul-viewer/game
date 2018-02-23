#ifndef __render_constant_buffer_h_included_
#define __render_constant_buffer_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"
#include "buffer.h"

namespace render {

template<typename buffer_struct>
class constant_buffer : protected buffer
{
public:
	void create( buffer_struct* in_data, bool in_is_dynamic = false );

	using buffer::add_ref;
	using buffer::release;

	using buffer::get_resource;

	void update( buffer_struct const& in_data ) const;
	void update( pcvoid in_data, u32 in_offset, u32 in_size ) const;

	void bind_vs( u32 in_slot ) const;
	void bind_ps( u32 in_slot ) const;
	void bind_gs( u32 in_slot ) const;
	void bind_hs( u32 in_slot ) const;
	void bind_ds( u32 in_slot ) const;
	void bind_cs( u32 in_slot ) const;
};

} // namespace render

#include "constant_buffer_inline.h"

#endif // #ifndef __render_constant_buffer_h_included_