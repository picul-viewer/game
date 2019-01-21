#ifndef __render_render_targets_h_included_
#define __render_render_targets_h_included_

#include <types.h>
#include "dx_include.h"

#include <math/vector.h>

#include "texture2d.h"
#include "resource_views.h"

namespace render {

class render_target_tex2d
{
public:
	void create( DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags );
	void create( texture2d const& in_texture, DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags );
	void destroy( );

	void bind_srv_vs( u32 in_slot ) const;
	void bind_srv_ps( u32 in_slot ) const;
	void bind_srv_gs( u32 in_slot ) const;
	void bind_srv_hs( u32 in_slot ) const;
	void bind_srv_ds( u32 in_slot ) const;
	void bind_srv_cs( u32 in_slot ) const;
	
	void bind_srv( u32 in_slot ) const;

	void bind_rtv( depth_stencil_view const& in_dsv ) const;

	void bind_uav( u32 in_slot ) const;
	
	inline texture2d&					get_texture	( ) { return m_texture; }
	inline shader_resource_view&		get_srv		( ) { return m_srv; }
	inline render_target_view&			get_rtv		( ) { return m_rtv; }
	inline unordered_access_view&		get_uav		( ) { return m_uav; }
	
	inline texture2d const&				get_texture	( ) const { return m_texture; }
	inline shader_resource_view const&	get_srv		( ) const { return m_srv; }
	inline render_target_view const&	get_rtv		( ) const { return m_rtv; }
	inline unordered_access_view const&	get_uav		( ) const { return m_uav; }

protected:
	void create_views( DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags );

	texture2d				m_texture;
	shader_resource_view	m_srv;
	render_target_view		m_rtv;
	unordered_access_view	m_uav;
};

class render_target_depth
{
public:
	void create( DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags );
	void destroy( );

	void bind_depth_srv_vs( u32 in_slot ) const;
	void bind_depth_srv_ps( u32 in_slot ) const;
	void bind_depth_srv_gs( u32 in_slot ) const;
	void bind_depth_srv_hs( u32 in_slot ) const;
	void bind_depth_srv_ds( u32 in_slot ) const;
	void bind_depth_srv_cs( u32 in_slot ) const;
	void bind_depth_srv( u32 in_slot ) const;
	
	void bind_stencil_srv_vs( u32 in_slot ) const;
	void bind_stencil_srv_ps( u32 in_slot ) const;
	void bind_stencil_srv_gs( u32 in_slot ) const;
	void bind_stencil_srv_hs( u32 in_slot ) const;
	void bind_stencil_srv_ds( u32 in_slot ) const;
	void bind_stencil_srv_cs( u32 in_slot ) const;
	void bind_stencil_srv( u32 in_slot ) const;

	inline texture2d&					get_texture			( ) { return m_texture; }
	inline shader_resource_view&		get_depth_srv		( ) { return m_depth_srv; }
	inline shader_resource_view&		get_stencil_srv		( ) { return m_stencil_srv; }
	inline depth_stencil_view&			get_dsv				( ) { return m_dsv; }
	inline depth_stencil_view&			get_const_dsv		( ) { return m_const_dsv; }
	
	inline texture2d const&				get_texture			( ) const { return m_texture; }
	inline shader_resource_view const&	get_depth_srv		( ) const { return m_depth_srv; }
	inline shader_resource_view const&	get_stencil_srv		( ) const { return m_stencil_srv; }
	inline depth_stencil_view const&	get_dsv				( ) const { return m_dsv; }
	inline depth_stencil_view const&	get_const_dsv		( ) const { return m_const_dsv; }

protected:
	texture2d				m_texture;
	shader_resource_view	m_depth_srv;
	shader_resource_view	m_stencil_srv;
	depth_stencil_view		m_dsv;
	depth_stencil_view		m_const_dsv;
};

} // namespace render

#endif // #ifndef __render_render_targets_h_included_