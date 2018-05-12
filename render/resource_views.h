#ifndef __render_resource_views_h_included_
#define __render_resource_views_h_included_

#include <types.h>
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
	
	shader_resource_view( );

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void destroy( );
	
	inline void set( ID3D11ShaderResourceView* in_view ) { m_srv = in_view; }
	inline ID3D11ShaderResourceView* const& get( ) const { return m_srv; }

	void bind_vs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_ps( u32 in_slot, u32 in_count = 1 ) const;
	void bind_gs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_hs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_ds( u32 in_slot, u32 in_count = 1 ) const;
	void bind_cs( u32 in_slot, u32 in_count = 1 ) const;
	
protected:
	ID3D11ShaderResourceView*	m_srv;
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
	
	depth_stencil_view( );

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void destroy( );
	
	inline void set( ID3D11DepthStencilView* in_view ) { m_dsv = in_view; }
	inline ID3D11DepthStencilView* const& get( ) const { return m_dsv; }

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
	
	render_target_view( );

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void destroy( );
	
	inline void set( ID3D11RenderTargetView* in_view ) { m_rtv = in_view; }
	inline ID3D11RenderTargetView* const& get( ) const { return m_rtv; }

	void bind( depth_stencil_view in_dsv, u32 in_count = 1 ) const;
	
protected:
	ID3D11RenderTargetView*	m_rtv;
};

} // namespace render

#endif // #ifndef __render_resource_views_h_included_