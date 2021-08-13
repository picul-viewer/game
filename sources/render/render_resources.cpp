#include "render_resources.h"
#include "resources.h"

namespace render {

void render_target_2d::create(
	DXGI_FORMAT in_format,
	math::u32x2 const in_size,
	D3D12_RESOURCE_STATES const in_initial_state,
	math::float4 const in_clear_value
)
{
	ASSERT( !format_is_depth( in_format ) );

	dx_resource::cook resource_cook;
	resource_cook.create_texture2d(
		in_format, in_size.x, in_size.y, 1, 1,
		true, false, true, false, false
	);
	resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
	resource_cook.set_initial_state( in_initial_state );
	resource_cook.set_clear_value( in_format, in_clear_value );
	m_resource.create( resource_cook );

	m_rtv = g_resources.create_rtv( m_resource, dx_rtv_create_texture2d( in_format ) );

	m_format = in_format;
}

void render_target_2d::destroy( )
{
	m_resource.destroy( );
}

dx_srv_cook render_target_2d::srv_cook( ) const
{
	return dx_srv_create_texture2d( m_format );
}


void depth_stencil_2d::create(
	DXGI_FORMAT in_format,
	math::u32x2 const in_size,
	D3D12_RESOURCE_STATES const in_initial_state,
	float const in_clear_depth,
	u8 const in_clear_stencil,
	bool const in_need_srv,
	bool const in_need_read_only_dsv
)
{
	ASSERT( format_is_depth( in_format ) );

	DXGI_FORMAT const resource_format = in_need_srv ? format_get_typeless( in_format ) : in_format;

	dx_resource::cook resource_cook;
	resource_cook.create_texture2d(
		resource_format, in_size.x, in_size.y, 1, 1,
		true, false, false, true, false
	);
	resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
	resource_cook.set_initial_state( in_initial_state );
	resource_cook.set_clear_value( in_format, in_clear_depth, in_clear_stencil );
	m_resource.create( resource_cook );

	m_dsv = g_resources.create_dsv( m_resource, dx_dsv_create_texture2d( in_format ) );
	if ( in_need_read_only_dsv )
		m_read_only_dsv = g_resources.create_dsv( m_resource, dx_dsv_create_texture2d( in_format, 0, true ) );

	m_format = format_depth_to_srv( in_format );
}

void depth_stencil_2d::destroy( )
{
	m_resource.destroy( );
}

dx_srv_cook depth_stencil_2d::srv_cook( ) const
{
	return dx_srv_create_texture2d( m_format );
}


void compute_target_2d::create(
	DXGI_FORMAT in_format,
	math::u32x2 const in_size,
	D3D12_RESOURCE_STATES const in_initial_state
)
{
	dx_resource::cook resource_cook;
	resource_cook.create_texture2d(
		in_format, in_size.x, in_size.y, 1, 1,
		true, true, false, false, false
	);
	resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
	resource_cook.set_initial_state( in_initial_state );
	m_resource.create( resource_cook );

	m_format = in_format;
}

void compute_target_2d::destroy( )
{
	m_resource.destroy( );
}

dx_uav_cook compute_target_2d::uav_cook( ) const
{
	return dx_uav_create_texture2d( m_format );
}

dx_srv_cook compute_target_2d::srv_cook( ) const
{
	return dx_srv_create_texture2d( m_format );
}

} // namespace render