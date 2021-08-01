#include "dx_views.h"
#include "dx.h"

namespace render {

dx_srv_cook dx_srv_create_buffer(
	DXGI_FORMAT const in_format,
	u64 const in_first_element,
	u32 const in_num_elements,
	u32 const in_element_stride,
	bool const in_is_raw
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.Buffer.FirstElement = in_first_element;
	result.desc.Buffer.NumElements = in_num_elements;
	result.desc.Buffer.StructureByteStride = in_element_stride;
	result.desc.Buffer.Flags = in_is_raw ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;
	return result;
}

dx_srv_cook dx_srv_create_texture1d(
	DXGI_FORMAT const in_format,
	u32 const in_first_mip,
	u32 const in_mip_count,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.Texture1D.MostDetailedMip = in_first_mip;
	result.desc.Texture1D.MipLevels = in_mip_count;
	result.desc.Texture1D.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

dx_srv_cook dx_srv_create_texture1d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_first_mip,
	u32 const in_mip_count,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.Texture1DArray.MostDetailedMip = in_first_mip;
	result.desc.Texture1DArray.MipLevels = in_mip_count;
	result.desc.Texture1DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture1DArray.ArraySize = in_slice_count;
	result.desc.Texture1DArray.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

dx_srv_cook dx_srv_create_texture2d(
	DXGI_FORMAT const in_format,
	u32 const in_first_mip,
	u32 const in_mip_count,
	u32 const in_plane_slice,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.Texture2D.MostDetailedMip = in_first_mip;
	result.desc.Texture2D.MipLevels = in_mip_count;
	result.desc.Texture2D.PlaneSlice = in_plane_slice;
	result.desc.Texture2D.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

dx_srv_cook dx_srv_create_texture2d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_first_mip,
	u32 const in_mip_count,
	u32 const in_plane_slice,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.Texture2DArray.MostDetailedMip = in_first_mip;
	result.desc.Texture2DArray.MipLevels = in_mip_count;
	result.desc.Texture2DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture2DArray.ArraySize = in_slice_count;
	result.desc.Texture2DArray.PlaneSlice = in_plane_slice;
	result.desc.Texture2DArray.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

dx_srv_cook dx_srv_create_texture3d(
	DXGI_FORMAT const in_format,
	u32 const in_first_mip,
	u32 const in_mip_count,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.Texture3D.MostDetailedMip = in_first_mip;
	result.desc.Texture3D.MipLevels = in_mip_count;
	result.desc.Texture3D.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

dx_srv_cook dx_srv_create_texturecube(
	DXGI_FORMAT const in_format,
	u32 const in_first_mip,
	u32 const in_mip_count,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.TextureCube.MostDetailedMip = in_first_mip;
	result.desc.TextureCube.MipLevels = in_mip_count;
	result.desc.TextureCube.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

dx_srv_cook dx_srv_create_texturecube_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_2d_slice,
	u32 const in_cube_count,
	u32 const in_first_mip,
	u32 const in_mip_count,
	float const in_min_lod_clamp
)
{
	dx_srv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
	result.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	result.desc.TextureCubeArray.MostDetailedMip = in_first_mip;
	result.desc.TextureCubeArray.MipLevels = in_mip_count;
    result.desc.TextureCubeArray.First2DArrayFace = in_first_2d_slice;
    result.desc.TextureCubeArray.NumCubes = in_cube_count;
	result.desc.TextureCubeArray.ResourceMinLODClamp = in_min_lod_clamp;
	return result;
}

void dx_srv_cook::build( dx_resource const in_resource, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const
{
	g_dx.device( )->CreateShaderResourceView( in_resource, &desc, in_handle );
}


dx_rtv_cook dx_rtv_create_buffer(
	DXGI_FORMAT const in_format,
	u64 const in_first_element,
	u32 const in_num_elements
)
{
	dx_rtv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_RTV_DIMENSION_BUFFER;
	result.desc.Buffer.FirstElement = in_first_element;
	result.desc.Buffer.NumElements = in_num_elements;
	return result;
}

dx_rtv_cook dx_rtv_create_texture1d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice
)
{
	dx_rtv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
	result.desc.Texture1D.MipSlice = in_mip_slice;
	return result;
}

dx_rtv_cook dx_rtv_create_texture1d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_mip_slice
)
{
	dx_rtv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
	result.desc.Texture1DArray.MipSlice = in_mip_slice;
	result.desc.Texture1DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture1DArray.ArraySize = in_slice_count;
	return result;
}

dx_rtv_cook dx_rtv_create_texture2d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice,
	u32 const in_plane_slice
)
{
	dx_rtv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	result.desc.Texture2D.MipSlice = in_mip_slice;
	result.desc.Texture2D.PlaneSlice = in_plane_slice;
	return result;
}

dx_rtv_cook dx_rtv_create_texture2d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_mip_slice,
	u32 const in_plane_slice
)
{
	dx_rtv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
	result.desc.Texture2DArray.MipSlice = in_mip_slice;
	result.desc.Texture2DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture2DArray.ArraySize = in_slice_count;
	result.desc.Texture2DArray.PlaneSlice = in_plane_slice;
	return result;
}

dx_rtv_cook dx_rtv_create_texture3d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice,
	u32 const in_first_slice,
	u32 const in_slice_count
)
{
	dx_rtv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
	result.desc.Texture3D.MipSlice = in_mip_slice;
	result.desc.Texture3D.FirstWSlice = in_first_slice;
	result.desc.Texture3D.WSize = in_slice_count;
	return result;
}

void dx_rtv_cook::build( dx_resource const in_resource, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const
{
	g_dx.device( )->CreateRenderTargetView( in_resource, &desc, in_handle );
}


dx_uav_cook dx_uav_create_buffer(
	DXGI_FORMAT const in_format,
	u64 const in_first_element,
	u32 const in_num_elements,
	u32 const in_element_stride,
	u64 const in_counter_byte_offset,
	bool const in_is_raw
)
{
	dx_uav_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	result.desc.Buffer.FirstElement = in_first_element;
	result.desc.Buffer.NumElements = in_num_elements;
    result.desc.Buffer.StructureByteStride = in_element_stride;
    result.desc.Buffer.CounterOffsetInBytes = in_counter_byte_offset;
    result.desc.Buffer.Flags = in_is_raw ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;
	return result;
}

dx_uav_cook dx_uav_create_texture1d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice
)
{
	dx_uav_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
	result.desc.Texture1D.MipSlice = in_mip_slice;
	return result;
}

dx_uav_cook dx_uav_create_texture1d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_mip_slice
)
{
	dx_uav_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
	result.desc.Texture1DArray.MipSlice = in_mip_slice;
	result.desc.Texture1DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture1DArray.ArraySize = in_slice_count;
	return result;
}

dx_uav_cook dx_uav_create_texture2d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice,
	u32 const in_plane_slice
)
{
	dx_uav_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	result.desc.Texture2D.MipSlice = in_mip_slice;
	result.desc.Texture2D.PlaneSlice = in_plane_slice;
	return result;
}

dx_uav_cook dx_uav_create_texture2d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_mip_slice,
	u32 const in_plane_slice
)
{
	dx_uav_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	result.desc.Texture2DArray.MipSlice = in_mip_slice;
	result.desc.Texture2DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture2DArray.ArraySize = in_slice_count;
	result.desc.Texture2DArray.PlaneSlice = in_plane_slice;
	return result;
}

dx_uav_cook dx_uav_create_texture3d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice,
	u32 const in_first_slice,
	u32 const in_slice_count
)
{
	dx_uav_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	result.desc.Texture3D.MipSlice = in_mip_slice;
	result.desc.Texture3D.FirstWSlice = in_first_slice;
	result.desc.Texture3D.WSize = in_slice_count;
	return result;
}

void dx_uav_cook::build( dx_resource const in_resource, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const
{
	g_dx.device( )->CreateUnorderedAccessView( in_resource, nullptr, &desc, in_handle );
}

void dx_uav_cook::build( dx_resource const in_resource, dx_resource const in_counter_buffer, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const
{
	g_dx.device( )->CreateUnorderedAccessView( in_resource, in_counter_buffer, &desc, in_handle );
}


dx_dsv_cook dx_dsv_create_texture1d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice,
	bool const in_read_only_depth,
	bool const in_read_only_stencil
)
{
	dx_dsv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
	result.desc.Flags =
		( in_read_only_depth ? D3D12_DSV_FLAG_READ_ONLY_DEPTH : D3D12_DSV_FLAG_NONE ) |
		( in_read_only_stencil ? D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE );
	result.desc.Texture1D.MipSlice = in_mip_slice;
	return result;
}

dx_dsv_cook dx_dsv_create_texture1d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_mip_slice,
	bool const in_read_only_depth,
	bool const in_read_only_stencil
)
{
	dx_dsv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
	result.desc.Flags =
		( in_read_only_depth ? D3D12_DSV_FLAG_READ_ONLY_DEPTH : D3D12_DSV_FLAG_NONE ) |
		( in_read_only_stencil ? D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE );
	result.desc.Texture1DArray.MipSlice = in_mip_slice;
	result.desc.Texture1DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture1DArray.ArraySize = in_slice_count;
	return result;
}

dx_dsv_cook dx_dsv_create_texture2d(
	DXGI_FORMAT const in_format,
	u32 const in_mip_slice,
	bool const in_read_only_depth,
	bool const in_read_only_stencil
)
{
	dx_dsv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	result.desc.Flags =
		( in_read_only_depth ? D3D12_DSV_FLAG_READ_ONLY_DEPTH : D3D12_DSV_FLAG_NONE ) |
		( in_read_only_stencil ? D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE );
	result.desc.Texture2D.MipSlice = in_mip_slice;
	return result;
}

dx_dsv_cook dx_dsv_create_texture2d_array(
	DXGI_FORMAT const in_format,
	u32 const in_first_slice,
	u32 const in_slice_count,
	u32 const in_mip_slice,
	bool const in_read_only_depth,
	bool const in_read_only_stencil
)
{
	dx_dsv_cook result;
	result.desc.Format = in_format;
	result.desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	result.desc.Flags =
		( in_read_only_depth ? D3D12_DSV_FLAG_READ_ONLY_DEPTH : D3D12_DSV_FLAG_NONE ) |
		( in_read_only_stencil ? D3D12_DSV_FLAG_READ_ONLY_STENCIL : D3D12_DSV_FLAG_NONE );
	result.desc.Texture2DArray.MipSlice = in_mip_slice;
	result.desc.Texture2DArray.FirstArraySlice = in_first_slice;
	result.desc.Texture2DArray.ArraySize = in_slice_count;
	return result;
}

void dx_dsv_cook::build( dx_resource const in_resource, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const
{
	g_dx.device( )->CreateDepthStencilView( in_resource, &desc, in_handle );
}

} // namespace render