#ifndef GUARD_RENDER_DX_VIEWS_H_INCLUDED
#define GUARD_RENDER_DX_VIEWS_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include "dx12.h"
#include "dx_resource.h"

namespace render {

struct dx_cbv_cook
{
	friend dx_cbv_cook dx_cbv_create(
		dx_resource const in_buffer,
		u32 const in_size
	);

	void build( D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const;

private:
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
};

struct dx_srv_cook
{
	friend dx_srv_cook dx_srv_create_buffer(
		DXGI_FORMAT const in_format,
		u64 const in_first_element,
		u32 const in_num_elements,
		u32 const in_element_stride,
		bool const in_is_raw = false
	);
	friend dx_srv_cook dx_srv_create_texture1d(
		DXGI_FORMAT const in_format,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		float const in_min_lod_clamp = 0.0f
	);
	friend dx_srv_cook dx_srv_create_texture1d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		float const in_min_lod_clamp = 0.0f
	);
	friend dx_srv_cook dx_srv_create_texture2d(
		DXGI_FORMAT const in_format,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		u32 const in_plane_slice = 0,
		float const in_min_lod_clamp = 0.0f
	);
	friend dx_srv_cook dx_srv_create_texture2d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		u32 const in_plane_slice = 0,
		float const in_min_lod_clamp = 0.0f
	);
	friend dx_srv_cook dx_srv_create_texture3d(
		DXGI_FORMAT const in_format,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		float const in_min_lod_clamp = 0.0f
	);
	friend dx_srv_cook dx_srv_create_texturecube(
		DXGI_FORMAT const in_format,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		float const in_min_lod_clamp = 0.0f
	);
	friend dx_srv_cook dx_srv_create_texturecube_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_2d_slice,
		u32 const in_cube_count,
		u32 const in_first_mip = 0,
		u32 const in_mip_count = -1,
		float const in_min_lod_clamp = 0.0f
	);

	void build( dx_resource const in_resouce, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const;

private:
	D3D12_SHADER_RESOURCE_VIEW_DESC desc;
};

struct dx_rtv_cook
{
	friend dx_rtv_cook dx_rtv_create_buffer(
		DXGI_FORMAT const in_format,
		u64 const in_first_element,
		u32 const in_num_elements
	);
	friend dx_rtv_cook dx_rtv_create_texture1d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0
	);
	friend dx_rtv_cook dx_rtv_create_texture1d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_mip_slice = 0
	);
	friend dx_rtv_cook dx_rtv_create_texture2d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0,
		u32 const in_plane_slice = 0
	);
	friend dx_rtv_cook dx_rtv_create_texture2d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_mip_slice = 0,
		u32 const in_plane_slice = 0
	);
	friend dx_rtv_cook dx_rtv_create_texture3d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0,
		u32 const in_first_slice = 0,
		u32 const in_slice_count = -1
	);

	void build( dx_resource const in_resouce, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const;

private:
	D3D12_RENDER_TARGET_VIEW_DESC desc;
};

struct dx_uav_cook
{
	friend dx_uav_cook dx_uav_create_buffer(
		DXGI_FORMAT const in_format,
		u64 const in_first_element,
		u32 const in_num_elements,
		u32 const in_element_stride,
		u64 const in_counter_byte_offset = 0,
		bool const in_is_raw = false
	);
	friend dx_uav_cook dx_uav_create_texture1d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0
	);
	friend dx_uav_cook dx_uav_create_texture1d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_mip_slice = 0
	);
	friend dx_uav_cook dx_uav_create_texture2d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0,
		u32 const in_plane_slice = 0
	);
	friend dx_uav_cook dx_uav_create_texture2d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_mip_slice = 0,
		u32 const in_plane_slice = 0
	);
	friend dx_uav_cook dx_uav_create_texture3d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0,
		u32 const in_first_slice = 0,
		u32 const in_slice_count = -1
	);

	void build( dx_resource const in_resouce, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const;
	void build( dx_resource const in_resouce, dx_resource const in_counter_buffer, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const;

private:
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
};

struct dx_dsv_cook
{
	friend dx_dsv_cook dx_dsv_create_texture1d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0,
		bool const in_read_only_depth = false,
		bool const in_read_only_stencil = false
	);
	friend dx_dsv_cook dx_dsv_create_texture1d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_mip_slice = 0,
		bool const in_read_only_depth = false,
		bool const in_read_only_stencil = false
	);
	friend dx_dsv_cook dx_dsv_create_texture2d(
		DXGI_FORMAT const in_format,
		u32 const in_mip_slice = 0,
		bool const in_read_only_depth = false,
		bool const in_read_only_stencil = false
	);
	friend dx_dsv_cook dx_dsv_create_texture2d_array(
		DXGI_FORMAT const in_format,
		u32 const in_first_slice,
		u32 const in_slice_count,
		u32 const in_mip_slice = 0,
		bool const in_read_only_depth = false,
		bool const in_read_only_stencil = false
	);

	void build( dx_resource const in_resouce, D3D12_CPU_DESCRIPTOR_HANDLE const in_handle ) const;

private:
	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_VIEWS_H_INCLUDED