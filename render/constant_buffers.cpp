#include "constant_buffers.h"
#include "render_api.h"

namespace render {

namespace constant_buffers
{
	struct
	{
		constant_buffer m_per_frame_constant_buffer;
		constant_buffer m_per_instance_constant_buffer;
	} m_data;

	void create( )
	{
		m_data.m_per_frame_constant_buffer.create( nullptr, per_frame_constant_buffer_size, true );
		m_data.m_per_instance_constant_buffer.create( nullptr, per_instance_constant_buffer_size, true );
	}

	void destroy( )
	{
		m_data.m_per_frame_constant_buffer.release( );
		m_data.m_per_instance_constant_buffer.release( );
	}

	constant_buffer const& get_per_frame_constant_buffer( )
	{
		return m_data.m_per_frame_constant_buffer;
	}

	constant_buffer const& get_per_instance_constant_buffer( )
	{
		return m_data.m_per_instance_constant_buffer;
	}

	void bind_vs( )
	{
		api::get_context( )->VSSetConstantBuffers( 0, default_constant_buffers_count, (ID3D11Buffer* const*)&m_data );
	}

	void bind_ps( )
	{
		api::get_context( )->PSSetConstantBuffers( 0, default_constant_buffers_count, (ID3D11Buffer* const*)&m_data );
	}

	void bind_gs( )
	{
		api::get_context( )->GSSetConstantBuffers( 0, default_constant_buffers_count, (ID3D11Buffer* const*)&m_data );
	}

	void bind_hs( )
	{
		api::get_context( )->HSSetConstantBuffers( 0, default_constant_buffers_count, (ID3D11Buffer* const*)&m_data );
	}

	void bind_ds( )
	{
		api::get_context( )->DSSetConstantBuffers( 0, default_constant_buffers_count, (ID3D11Buffer* const*)&m_data );
	}

	void bind_cs( )
	{
		api::get_context( )->CSSetConstantBuffers( 0, default_constant_buffers_count, (ID3D11Buffer* const*)&m_data );
	}

	void bind( )
	{
		bind_vs( );
		bind_ps( );
		bind_gs( );
		bind_hs( );
		bind_ds( );
		bind_cs( );
	}

} // namespace constant_buffers

} // namespace render