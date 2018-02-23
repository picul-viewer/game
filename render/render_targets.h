#ifndef __render_render_targets_h_included_
#define __render_render_targets_h_included_

#include <types.h>
#include "dx_include.h"

#include "buffer.h"
#include "texture.h"
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
	
	inline shader_resource_view	const& get_srv		( ) const { return m_srv; }
	inline render_target_view	const& get_rtv		( ) const { return m_rtv; }
	inline depth_stencil_view	const& get_dsv		( ) const { return m_dsv; }
	inline depth_stencil_view	const& get_const_dsv( ) const { return m_const_dsv; }

protected:
	void create_views( DXGI_FORMAT in_format, math::u16x2 in_size, u32 in_bind_flags );

	texture2d				m_texture;
	shader_resource_view	m_srv;
	render_target_view		m_rtv;
	depth_stencil_view		m_dsv;
	depth_stencil_view		m_const_dsv;
	bool					m_destroy_texture;
};

} // namespace render

#endif // #ifndef __render_render_targets_h_included_