#include "render_targets.h"

namespace render {

void render_target_tex2d::create_views( DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags )
{
	if ( in_bind_flags & D3D11_BIND_SHADER_RESOURCE )
	{
		shader_resource_view::cook cook;
		cook.set_tex2d_srv( in_format, 0, 1 );
		m_srv.create( m_texture.get( ), cook );
	}

	if ( in_bind_flags & D3D11_BIND_RENDER_TARGET )
	{
		render_target_view::cook cook;
		cook.set_tex2d_rtv( in_format, 0 );
		m_rtv.create( m_texture.get( ), cook );
	}
	
	if ( in_bind_flags & D3D11_BIND_DEPTH_STENCIL )
	{
		depth_stencil_view::cook cook;
		cook.set_tex2d_dsv( in_format, 0, 0 );
		m_dsv.create( m_texture.get( ), cook );
		depth_stencil_view::cook const_cook;
		const_cook.set_tex2d_dsv( in_format, 0, D3D11_DSV_READ_ONLY_DEPTH & D3D11_DSV_READ_ONLY_STENCIL );
		m_const_dsv.create( m_texture.get( ), const_cook );
	}
}

void render_target_tex2d::create( DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags )
{
	texture2d::cook texture_cook;
	texture_cook.set_texture2d( in_size.x, in_size.y, 1, 1, in_format,
								1, 0, D3D11_USAGE_DEFAULT, in_bind_flags, 0, 0 );
	m_texture.create	( texture_cook );

	create_views( in_format, in_size, in_bind_flags );
}

void render_target_tex2d::create( texture2d const& in_texture, DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags )
{
	m_texture			= in_texture;
	m_texture.add_ref	( );

	create_views( in_format, in_size, in_bind_flags );
}

void render_target_tex2d::destroy( )
{
	m_texture.release( );
	m_srv.destroy( );
	m_rtv.destroy( );
	m_dsv.destroy( );
	m_const_dsv.destroy( );
}

void render_target_tex2d::bind_srv_vs( u32 in_slot ) const
{
	m_srv.bind_vs( in_slot );
}

void render_target_tex2d::bind_srv_ps( u32 in_slot ) const
{
	m_srv.bind_ps( in_slot );
}

void render_target_tex2d::bind_srv_gs( u32 in_slot ) const
{
	m_srv.bind_gs( in_slot );
}

void render_target_tex2d::bind_srv_hs( u32 in_slot ) const
{
	m_srv.bind_hs( in_slot );
}

void render_target_tex2d::bind_srv_ds( u32 in_slot ) const
{
	m_srv.bind_ds( in_slot );
}

void render_target_tex2d::bind_srv_cs( u32 in_slot ) const
{
	m_srv.bind_cs( in_slot );
}

void render_target_tex2d::bind_srv( u32 in_slot ) const
{
	bind_srv_vs( in_slot );
	bind_srv_ps( in_slot );
	bind_srv_gs( in_slot );
	bind_srv_hs( in_slot );
	bind_srv_ds( in_slot );
	bind_srv_cs( in_slot );
}

void render_target_tex2d::bind_rtv( depth_stencil_view const& in_dsv ) const
{
	m_rtv.bind( in_dsv );
}

} // namespace render