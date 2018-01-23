#ifndef __render_constant_buffer_inline_h_included_
#define __render_constant_buffer_inline_h_included_

#include "render_core.h"

namespace render {

template<typename buffer_struct>
void typed_constant_buffer<buffer_struct>::create( buffer_struct* in_data, bool in_is_dynamic )
{
	create( in_data, sizeof(buffer_struct), in_is_dynamic );
}

template<typename buffer_struct>
void typed_constant_buffer<buffer_struct>::update( buffer_struct const& in_data ) const
{
	update( &in_data, 0, sizeof(buffer_struct) );
}

} // namespace render

#endif // #ifndef __render_constant_buffer_inline_h_included_