#ifndef __render_constant_buffer_inline_h_included_
#define __render_constant_buffer_inline_h_included_

#include "render_core.h"

namespace render {

template<typename buffer_struct>
void constant_buffer<buffer_struct>::create( buffer_struct* in_data, bool in_is_dynamic )
{
	buffer::cook buffer_cook;
	buffer_cook.set_constant_buffer( sizeof(buffer_struct), in_is_dynamic );
	buffer::create( buffer_cook, in_data );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::update( buffer_struct const& in_data ) const
{
	buffer::update_dynamic( &in_data, 0, sizeof(buffer_struct) );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::update( pcvoid in_data, u32 in_offset, u32 in_size ) const
{
	buffer::update_dynamic( in_data, in_offset, in_size );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::bind_vs( u32 in_slot ) const
{
	api::get_context( )->VSSetConstantBuffers( in_slot, 1, &m_buffer );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::bind_ps( u32 in_slot ) const
{
	api::get_context( )->PSSetConstantBuffers( in_slot, 1, &m_buffer );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::bind_gs( u32 in_slot ) const
{
	api::get_context( )->GSSetConstantBuffers( in_slot, 1, &m_buffer );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::bind_hs( u32 in_slot ) const
{
	api::get_context( )->HSSetConstantBuffers( in_slot, 1, &m_buffer );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::bind_ds( u32 in_slot ) const
{
	api::get_context( )->DSSetConstantBuffers( in_slot, 1, &m_buffer );
}

template<typename buffer_struct>
void constant_buffer<buffer_struct>::bind_cs( u32 in_slot ) const
{
	api::get_context( )->CSSetConstantBuffers( in_slot, 1, &m_buffer );
}

} // namespace render

#endif // #ifndef __render_constant_buffer_inline_h_included_