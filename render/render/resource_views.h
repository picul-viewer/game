#ifndef __render_resource_views_h_included_
#define __render_resource_views_h_included_

#include <core/types.h>
#include "dx_include.h"

namespace render {

class shader_resource_view
{
public:
	struct cook
	{
		void set_buffer_srv(
			DXGI_FORMAT	in_format,
			u32			in_offset,
			u32			in_width
		);

		void set_tex1d_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1
		);
		
		void set_tex1d_array_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex2d_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1
		);
		
		void set_tex2d_array_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex2dms_srv(
			DXGI_FORMAT	in_format
		);
		
		void set_tex2dms_array_srv(
			DXGI_FORMAT	in_format,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex3d_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1
		);
		
		void set_tex_cube_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1
		);
		
		void set_tex_cube_array_srv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_mip_count	= (u32)-1,
			u32			in_first_face	= 0,
			u32			in_array_size	= (u32)-1
		);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	};

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void set( ID3D11ShaderResourceView* in_view );
	void destroy( );
	
	void bind_vs( u32 in_slot ) const;
	void bind_ps( u32 in_slot ) const;
	void bind_gs( u32 in_slot ) const;
	void bind_hs( u32 in_slot ) const;
	void bind_ds( u32 in_slot ) const;
	void bind_cs( u32 in_slot ) const;

	inline ID3D11ShaderResourceView* get_view( ) const { return m_srv; }

protected:
	ID3D11ShaderResourceView*	m_srv;
};

class shader_resource_views
{
public:
	enum { max_shader_resources = 8 };

	void set_shader_resource( u32 index, shader_resource_view in_view );
	void destroy( );

	shader_resource_view& operator[]( u32 index );
	
	void bind_vs( ) const;
	void bind_ps( ) const;
	void bind_gs( ) const;
	void bind_hs( ) const;
	void bind_ds( ) const;
	void bind_cs( ) const;

protected:
	shader_resource_view	m_shader_resources[max_shader_resources]{};
};

class depth_stencil_view
{
public:
	struct cook
	{
		void set_tex1d_dsv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_flags		= 0
		);
		
		void set_tex1d_array_dsv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1,
			u32			in_flags		= 0
		);
		
		void set_tex2d_dsv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_flags		= 0
		);
		
		void set_tex2d_array_dsv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1,
			u32			in_flags		= 0
		);
		
		void set_tex2dms_dsv(
			DXGI_FORMAT	in_format,
			u32			in_flags		= 0
		);
		
		void set_tex2dms_array_dsv(
			DXGI_FORMAT	in_format,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1,
			u32			in_flags		= 0
		);
		
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	};

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void set( ID3D11DepthStencilView* in_view );
	void destroy( );
	
	inline ID3D11DepthStencilView* get_view( ) const { return m_dsv; }

protected:
	ID3D11DepthStencilView*	m_dsv;
};

class render_target_view
{
public:
	struct cook
	{
		void set_buffer_rtv(
			DXGI_FORMAT	in_format,
			u32			in_offset,
			u32			in_width
		);

		void set_tex1d_rtv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0
		);
		
		void set_tex1d_array_rtv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex2d_rtv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0
		);
		
		void set_tex2d_array_rtv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex2dms_rtv(
			DXGI_FORMAT	in_format
		);
		
		void set_tex2dms_array_rtv(
			DXGI_FORMAT	in_format,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex3d_rtv(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0
		);

		D3D11_RENDER_TARGET_VIEW_DESC desc;
	};

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void set( ID3D11RenderTargetView* in_view );
	void destroy( );
	
	void bind( depth_stencil_view const& in_dsv ) const;
	
	inline ID3D11RenderTargetView* get_view( ) const { return m_rtv; }

protected:
	ID3D11RenderTargetView*	m_rtv;
};

class render_target_views
{
public:
	enum { max_render_targets = 4 };

	void set_render_target( u32 index, render_target_view in_view );
	void set_depth_stencil( depth_stencil_view in_view );
	void destroy( );

	render_target_view& get_render_target( u32 index );
	depth_stencil_view& get_depth_stencil( );

	void bind( ) const;

protected:
	render_target_view	m_render_targets[max_render_targets];
	depth_stencil_view	m_depth_stencil;
};

} // namespace render

#endif // #ifndef __render_resource_views_h_included_