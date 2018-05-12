#include "constant_buffer.h"

#include "render_api.h"

namespace render {

void constant_buffer::create( pvoid in_data, uptr in_size, bool in_is_dynamic )
{
	buffer::cook buffer_cook;
	buffer_cook.set_constant_buffer( (u32)in_size, in_is_dynamic );
	m_buffer.create( buffer_cook, in_data );
}

void constant_buffer::destroy( )
{
	m_buffer.destroy( );
}

u32 constant_buffer::add_ref( ) const
{
	return m_buffer.add_ref( );
}

u32 constant_buffer::release( ) const
{
	return m_buffer.release( );
}
	
void constant_buffer::update( pcvoid in_data, u32 in_offset, u32 in_size ) const
{
	m_buffer.update_dynamic( in_data, in_offset, in_size );
}

void constant_buffer::bind_vs( u32 in_slot ) const
{
	api::get_context( )->VSSetConstantBuffers( in_slot, 1, &get( ) );
}

void constant_buffer::bind_ps( u32 in_slot ) const
{
	api::get_context( )->PSSetConstantBuffers( in_slot, 1, &get( ) );
}

void constant_buffer::bind_gs( u32 in_slot ) const
{
	api::get_context( )->GSSetConstantBuffers( in_slot, 1, &get( ) );
}

void constant_buffer::bind_hs( u32 in_slot ) const
{
	api::get_context( )->HSSetConstantBuffers( in_slot, 1, &get( ) );
}

void constant_buffer::bind_ds( u32 in_slot ) const
{
	api::get_context( )->DSSetConstantBuffers( in_slot, 1, &get( ) );
}

void constant_buffer::bind_cs( u32 in_slot ) const
{
	api::get_context( )->CSSetConstantBuffers( in_slot, 1, &get( ) );
}

} // namespace render