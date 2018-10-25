#include "resource_views.h"
#include "api.h"
#include <macros.h>

namespace render {

void shader_resource_view::cook::set_buffer_srv(
	DXGI_FORMAT	in_format,
	u32			in_offset,
	u32			in_width )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement				= in_offset;
	desc.Buffer.NumElements					= in_width;
}

void shader_resource_view::cook::set_tex1d_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE1D;
	desc.Texture1D.MostDetailedMip			= in_mip_slice;
	desc.Texture1D.MipLevels				= in_mip_count;
}

void shader_resource_view::cook::set_tex1d_array_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count,
	u32			in_array_first,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
	desc.Texture1DArray.MostDetailedMip		= in_mip_slice;
	desc.Texture1DArray.MipLevels			= in_mip_count;
	desc.Texture1DArray.FirstArraySlice		= in_array_first;
	desc.Texture1DArray.ArraySize			= in_array_size;
}

void shader_resource_view::cook::set_tex2d_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MostDetailedMip			= in_mip_slice;
	desc.Texture2D.MipLevels				= in_mip_count;
}

void shader_resource_view::cook::set_tex2d_array_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count,
	u32			in_array_first,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MostDetailedMip		= in_mip_slice;
	desc.Texture2DArray.MipLevels			= in_mip_count;
	desc.Texture2DArray.FirstArraySlice		= in_array_first;
	desc.Texture2DArray.ArraySize			= in_array_size;
}

void shader_resource_view::cook::set_tex2dms_srv( DXGI_FORMAT in_format )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE2DMS;
}

void shader_resource_view::cook::set_tex2dms_array_srv(
	DXGI_FORMAT	in_format,
	u32			in_array_first,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
	desc.Texture2DMSArray.FirstArraySlice	= in_array_first;
	desc.Texture2DMSArray.ArraySize			= in_array_size;
}

void shader_resource_view::cook::set_tex3d_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE3D;
	desc.Texture3D.MostDetailedMip			= in_mip_slice;
	desc.Texture3D.MipLevels				= in_mip_count;
}

void shader_resource_view::cook::set_tex_cube_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURECUBE;
	desc.TextureCube.MostDetailedMip		= in_mip_slice;
	desc.TextureCube.MipLevels				= in_mip_count;
}

void shader_resource_view::cook::set_tex_cube_array_srv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_mip_count,
	u32			in_first_face,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	desc.TextureCubeArray.MostDetailedMip	= in_mip_slice;
	desc.TextureCubeArray.MipLevels			= in_mip_count;
	desc.TextureCubeArray.First2DArrayFace	= in_first_face;
	desc.TextureCubeArray.NumCubes			= in_array_size;
}

shader_resource_view::shader_resource_view( ) :
	m_srv( nullptr )
{ }

void shader_resource_view::create( ID3D11Resource* in_resource, cook const& in_cook )
{
	g_api.get_device( )->CreateShaderResourceView( in_resource, &in_cook.desc, &m_srv );
}

void shader_resource_view::destroy( )
{
	dx_release( m_srv );
}

void shader_resource_view::bind_vs( u32 in_slot, u32 in_count ) const
{
	g_api.get_context( )->VSSetShaderResources( in_slot, in_count, &m_srv );
}

void shader_resource_view::bind_ps( u32 in_slot, u32 in_count ) const
{
	g_api.get_context( )->PSSetShaderResources( in_slot, in_count, &m_srv );
}

void shader_resource_view::bind_gs( u32 in_slot, u32 in_count ) const
{
	g_api.get_context( )->GSSetShaderResources( in_slot, in_count, &m_srv );
}

void shader_resource_view::bind_hs( u32 in_slot, u32 in_count ) const
{
	g_api.get_context( )->HSSetShaderResources( in_slot, in_count, &m_srv );
}

void shader_resource_view::bind_ds( u32 in_slot, u32 in_count ) const
{
	g_api.get_context( )->DSSetShaderResources( in_slot, in_count, &m_srv );
}

void shader_resource_view::bind_cs( u32 in_slot, u32 in_count ) const
{
	g_api.get_context( )->CSSetShaderResources( in_slot, in_count, &m_srv );
}


void depth_stencil_view::cook::set_tex1d_dsv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_flags )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_DSV_DIMENSION_TEXTURE1D;
	desc.Flags								= in_flags;
	desc.Texture1D.MipSlice					= in_mip_slice;
}

void depth_stencil_view::cook::set_tex1d_array_dsv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_array_first,
	u32			in_array_size,
	u32			in_flags )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_DSV_DIMENSION_TEXTURE1D;
	desc.Flags								= in_flags;
	desc.Texture1DArray.MipSlice			= in_mip_slice;
	desc.Texture1DArray.FirstArraySlice		= in_array_first;
	desc.Texture1DArray.ArraySize			= in_array_size;
}

void depth_stencil_view::cook::set_tex2d_dsv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_flags )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Flags								= in_flags;
	desc.Texture2D.MipSlice					= in_mip_slice;
}

void depth_stencil_view::cook::set_tex2d_array_dsv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_array_first,
	u32			in_array_size,
	u32			in_flags )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	desc.Flags								= in_flags;
	desc.Texture2DArray.MipSlice			= in_mip_slice;
	desc.Texture2DArray.FirstArraySlice		= in_array_first;
	desc.Texture2DArray.ArraySize			= in_array_size;
}

void depth_stencil_view::cook::set_tex2dms_dsv(
	DXGI_FORMAT in_format,
	u32			in_flags )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_DSV_DIMENSION_TEXTURE2DMS;
	desc.Flags								= in_flags;
}

void depth_stencil_view::cook::set_tex2dms_array_dsv(
	DXGI_FORMAT	in_format,
	u32			in_array_first,
	u32			in_array_size,
	u32			in_flags )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
	desc.Flags								= in_flags;
	desc.Texture2DMSArray.FirstArraySlice	= in_array_first;
	desc.Texture2DMSArray.ArraySize			= in_array_size;
}

depth_stencil_view::depth_stencil_view( ) :
	m_dsv( nullptr )
{ }

void depth_stencil_view::create( ID3D11Resource* in_resource, cook const& in_cook )
{
	g_api.get_device( )->CreateDepthStencilView( in_resource, &in_cook.desc, &m_dsv );
}

void depth_stencil_view::destroy( )
{
	dx_release( m_dsv );
}

void depth_stencil_view::clear( float in_depth_value ) const
{
	g_api.get_context( )->ClearDepthStencilView( m_dsv, D3D11_CLEAR_DEPTH, in_depth_value, 0u );
}

void depth_stencil_view::clear( u8 in_stencil_value ) const
{
	g_api.get_context( )->ClearDepthStencilView( m_dsv, D3D11_CLEAR_STENCIL, 0.0f, in_stencil_value );
}

void depth_stencil_view::clear( float in_depth_value, u8 in_stencil_value ) const
{
	g_api.get_context( )->ClearDepthStencilView( m_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, in_depth_value, in_stencil_value );
}


void render_target_view::cook::set_buffer_rtv(
	DXGI_FORMAT	in_format,
	u32			in_offset,
	u32			in_width )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement				= in_offset;
	desc.Buffer.NumElements					= in_width;
}

void render_target_view::cook::set_tex1d_rtv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE1D;
	desc.Texture1D.MipSlice					= in_mip_slice;
}

void render_target_view::cook::set_tex1d_array_rtv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_array_first,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
	desc.Texture1DArray.MipSlice			= in_mip_slice;
	desc.Texture1DArray.FirstArraySlice		= in_array_first;
	desc.Texture1DArray.ArraySize			= in_array_size;
}

void render_target_view::cook::set_tex2d_rtv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice					= in_mip_slice;
}

void render_target_view::cook::set_tex2d_array_rtv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice,
	u32			in_array_first,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MipSlice			= in_mip_slice;
	desc.Texture2DArray.FirstArraySlice		= in_array_first;
	desc.Texture2DArray.ArraySize			= in_array_size;
}

void render_target_view::cook::set_tex2dms_rtv( DXGI_FORMAT in_format )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE2DMS;
}

void render_target_view::cook::set_tex2dms_array_rtv(
	DXGI_FORMAT	in_format,
	u32			in_array_first,
	u32			in_array_size )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
	desc.Texture2DMSArray.FirstArraySlice	= in_array_first;
	desc.Texture2DMSArray.ArraySize			= in_array_size;
}

void render_target_view::cook::set_tex3d_rtv(
	DXGI_FORMAT	in_format,
	u32			in_mip_slice )
{
	desc.Format								= in_format;
	desc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE3D;
	desc.Texture3D.MipSlice					= in_mip_slice;
}

render_target_view::render_target_view( ) :
	m_rtv( nullptr )
{ }

void render_target_view::create( ID3D11Resource* in_resource, cook const& in_cook )
{
	g_api.get_device( )->CreateRenderTargetView( in_resource, &in_cook.desc, &m_rtv );
}

void render_target_view::destroy( )
{
	dx_release( m_rtv );
}

void render_target_view::bind( depth_stencil_view in_dsv, u32 in_count ) const
{
	g_api.get_context( )->OMSetRenderTargets( in_count, &m_rtv, in_dsv.get( ) );
}

void render_target_view::clear( math::float4 const& in_value ) const
{
	g_api.get_context( )->ClearRenderTargetView( m_rtv, in_value.data );
}

} // namespace render