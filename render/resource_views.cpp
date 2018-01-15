#include "resource_views.h"
#include "render_api.h"

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

void shader_resource_view::create( ID3D11Resource* in_resource, cook const& in_cook )
{
	api::get_device( )->CreateShaderResourceView( in_resource, &in_cook.desc, &m_srv );
}

void shader_resource_view::set( ID3D11ShaderResourceView* in_view )
{
	m_srv = in_view;
}

void shader_resource_view::destroy( )
{
	dx_release( m_srv );
}

void shader_resource_view::bind_vs( u32 in_slot ) const
{
	api::get_context( )->VSSetShaderResources( in_slot, 1, &m_srv );
}

void shader_resource_view::bind_ps( u32 in_slot ) const
{
	api::get_context( )->PSSetShaderResources( in_slot, 1, &m_srv );
}

void shader_resource_view::bind_gs( u32 in_slot ) const
{
	api::get_context( )->GSSetShaderResources( in_slot, 1, &m_srv );
}

void shader_resource_view::bind_hs( u32 in_slot ) const
{
	api::get_context( )->HSSetShaderResources( in_slot, 1, &m_srv );
}

void shader_resource_view::bind_ds( u32 in_slot ) const
{
	api::get_context( )->DSSetShaderResources( in_slot, 1, &m_srv );
}

void shader_resource_view::bind_cs( u32 in_slot ) const
{
	api::get_context( )->CSSetShaderResources( in_slot, 1, &m_srv );
}


void shader_resource_views::set_shader_resource( u32 index, shader_resource_view in_view )
{
	ASSERT( index < max_shader_resources );
	m_shader_resources[index] = in_view;
}

void shader_resource_views::destroy( )
{
	for ( u32 i = 0; i < max_shader_resources; ++i )
		m_shader_resources[i].destroy( );
}

shader_resource_view& shader_resource_views::operator[]( u32 index )
{
	ASSERT( index < max_shader_resources );
	return m_shader_resources[index];
}

void shader_resource_views::bind_vs( ) const
{
	api::get_context( )->VSSetShaderResources(
		0,
		max_shader_resources,
		(ID3D11ShaderResourceView*const*)m_shader_resources
	);
}

void shader_resource_views::bind_ps() const
{
	api::get_context( )->PSSetShaderResources(
		0,
		max_shader_resources,
		(ID3D11ShaderResourceView*const*)m_shader_resources
	);
}

void shader_resource_views::bind_gs() const
{
	api::get_context( )->GSSetShaderResources(
		0,
		max_shader_resources,
		(ID3D11ShaderResourceView*const*)m_shader_resources
	);
}

void shader_resource_views::bind_hs() const
{
	api::get_context( )->HSSetShaderResources(
		0,
		max_shader_resources,
		(ID3D11ShaderResourceView*const*)m_shader_resources
	);
}

void shader_resource_views::bind_ds() const
{
	api::get_context( )->DSSetShaderResources(
		0,
		max_shader_resources,
		(ID3D11ShaderResourceView*const*)m_shader_resources
	);
}

void shader_resource_views::bind_cs() const
{
	api::get_context( )->CSSetShaderResources(
		0,
		max_shader_resources,
		(ID3D11ShaderResourceView*const*)m_shader_resources
	);
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

void depth_stencil_view::create( ID3D11Resource* in_resource, cook const& in_cook )
{
	api::get_device( )->CreateDepthStencilView( in_resource, &in_cook.desc, &m_dsv );
}

void depth_stencil_view::set( ID3D11DepthStencilView* in_view )
{
	m_dsv = in_view;
}

void depth_stencil_view::destroy( )
{
	dx_release( m_dsv );
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

void render_target_view::create( ID3D11Resource* in_resource, cook const& in_cook )
{
	api::get_device( )->CreateRenderTargetView( in_resource, &in_cook.desc, &m_rtv );
}

void render_target_view::set( ID3D11RenderTargetView* in_view )
{
	m_rtv = in_view;
}

void render_target_view::destroy( )
{
	dx_release( m_rtv );
}

void render_target_view::bind( depth_stencil_view const& in_dsv ) const
{
	api::get_context( )->OMSetRenderTargets( 1, &m_rtv, (ID3D11DepthStencilView*)&in_dsv );
}


void render_target_views::set_render_target( u32 index, render_target_view in_view )
{
	ASSERT( index < max_render_targets );
	m_render_targets[index] = in_view;
}

void render_target_views::set_depth_stencil( depth_stencil_view in_view )
{
	m_depth_stencil = in_view;
}

void render_target_views::destroy( )
{
	for ( u32 i = 0; i < max_render_targets; ++i )
		m_render_targets[i].destroy( );
	m_depth_stencil.destroy( );
}

render_target_view& render_target_views::get_render_target( u32 index )
{
	ASSERT( index < max_render_targets );
	return m_render_targets[index];
}

depth_stencil_view& render_target_views::get_depth_stencil( )
{
	return m_depth_stencil;
}

void render_target_views::bind( ) const
{
	api::get_context( )->OMSetRenderTargets(
		max_render_targets,
		(ID3D11RenderTargetView*const*)m_render_targets,
		m_depth_stencil.get_view( )
	);
}

} // namespace render