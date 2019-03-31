#ifndef GUARD_RENDER_RESOURCE_VIEWS_H_INCLUDED
#define GUARD_RENDER_RESOURCE_VIEWS_H_INCLUDED

#include <types.h>
#include "dx_include.h"
#include <math/vector.h>

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
	
	void clear( float in_depth_value ) const;
	void clear( u8 in_stencil_value ) const;
	void clear( float in_depth_value, u8 in_stencil_value ) const;

protected:
	ID3D11DepthStencilView*	m_dsv;
};

class unordered_access_view
{
public:
	struct cook
	{
		void set_buffer_uav(
			DXGI_FORMAT	in_format,
			u32			in_offset,
			u32			in_width,
			u32			in_flags
		);

		void set_tex1d_uav(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0
		);
		
		void set_tex1d_array_uav(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex2d_uav(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0
		);
		
		void set_tex2d_array_uav(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_array_first	= 0,
			u32			in_array_size	= (u32)-1
		);
		
		void set_tex3d_uav(
			DXGI_FORMAT	in_format,
			u32			in_mip_slice	= 0,
			u32			in_slice_first	= 0,
			u32			in_slice_size	= (u32)-1
		);
		
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	};
	
	unordered_access_view( );

	void create( ID3D11Resource* in_resource, cook const& in_cook );
	void destroy( );
	
	inline void set( ID3D11UnorderedAccessView* in_view ) { m_uav = in_view; }
	inline ID3D11UnorderedAccessView* const& get( ) const { return m_uav; }

	void bind_cs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_cs( u32 in_slot, u32* in_initial_counts, u32 in_count = 1 ) const;
	void clear( math::float4 const& in_value ) const;
	void clear( math::u32x4 const& in_value ) const;
	
protected:
	ID3D11UnorderedAccessView*	m_uav;
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
	
	void bind( u32 in_count = 1 ) const;
	void bind( depth_stencil_view in_dsv, u32 in_count = 1 ) const;
	void clear( math::float4 const& in_value ) const;
	
protected:
	ID3D11RenderTargetView*	m_rtv;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_RESOURCE_VIEWS_H_INCLUDED