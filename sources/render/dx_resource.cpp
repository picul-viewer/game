#include "dx_resource.h"
#include "dx.h"

namespace render {

void dx_resource::cook::create_buffer(
	u64 const in_size,
	bool const in_allow_shader_resource,
	bool const in_allow_unordered_access,
	bool const in_allow_render_target,
	bool const in_allow_simultaneous_access
)
{
	// For committed resource.
	heap_flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = in_size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	desc.Flags = D3D12_RESOURCE_FLAG_NONE |
		( in_allow_shader_resource ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ) |
		( in_allow_unordered_access ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_render_target ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_simultaneous_access ? D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void dx_resource::cook::create_texture1d(
	DXGI_FORMAT const in_format,
	u32 const in_width,
	u32 const in_array_size,
	u32 const in_mip_levels,
	bool const in_allow_shader_resource,
	bool const in_allow_unordered_access,
	bool const in_allow_render_target,
	bool const in_allow_depth_stencil,
	bool const in_allow_simultaneous_access
)
{
	// For committed resource.
	heap_flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	desc.Alignment = 0;
	desc.Width = in_width;
	desc.Height = 1;
	desc.DepthOrArraySize = in_array_size;
	desc.MipLevels = in_mip_levels;
	desc.Format = in_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	desc.Flags = D3D12_RESOURCE_FLAG_NONE |
		( in_allow_shader_resource ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ) |
		( in_allow_unordered_access ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_render_target ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_depth_stencil ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_simultaneous_access ? D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void dx_resource::cook::create_texture2d(
	DXGI_FORMAT const in_format,
	u32 const in_width,
	u32 const in_height,
	u32 const in_array_size,
	u32 const in_mip_levels,
	bool const in_allow_shader_resource,
	bool const in_allow_unordered_access,
	bool const in_allow_render_target,
	bool const in_allow_depth_stencil,
	bool const in_allow_simultaneous_access
)
{
	// For committed resource.
	heap_flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = in_width;
	desc.Height = in_height;
	desc.DepthOrArraySize = in_array_size;
	desc.MipLevels = in_mip_levels;
	desc.Format = in_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	desc.Flags = D3D12_RESOURCE_FLAG_NONE |
		( in_allow_shader_resource ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ) |
		( in_allow_unordered_access ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_render_target ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_depth_stencil ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_simultaneous_access ? D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void dx_resource::cook::create_texture3d(
	DXGI_FORMAT const in_format,
	u32 const in_width,
	u32 const in_height,
	u32 const in_depth,
	u32 const in_mip_levels,
	bool const in_allow_shader_resource,
	bool const in_allow_unordered_access,
	bool const in_allow_render_target,
	bool const in_allow_depth_stencil,
	bool const in_allow_simultaneous_access
)
{
	// For committed resource.
	heap_flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	desc.Alignment = 0;
	desc.Width = in_width;
	desc.Height = in_height;
	desc.DepthOrArraySize = in_depth;
	desc.MipLevels = in_mip_levels;
	desc.Format = in_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	desc.Flags = D3D12_RESOURCE_FLAG_NONE |
		( in_allow_shader_resource ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ) |
		( in_allow_unordered_access ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_render_target ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_depth_stencil ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_simultaneous_access ? D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void dx_resource::cook::set_heap_type( D3D12_HEAP_TYPE const in_heap_type )
{
	heap_props.Type = in_heap_type;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_props.CreationNodeMask = 0;
	heap_props.VisibleNodeMask = 0;
}

void dx_resource::cook::set_initial_state( D3D12_RESOURCE_STATES const in_state )
{
	initial_state = in_state;
}

void dx_resource::cook::set_clear_value( DXGI_FORMAT const in_format, math::float4 const in_value )
{
	has_optimal_clear = true;
	optimal_clear.Format = in_format;
	optimal_clear.Color[0] = in_value.data[0];
	optimal_clear.Color[1] = in_value.data[1];
	optimal_clear.Color[2] = in_value.data[2];
	optimal_clear.Color[3] = in_value.data[3];
}

void dx_resource::cook::set_clear_value( DXGI_FORMAT const in_format, float const in_depth, u8 const in_stencil )
{
	has_optimal_clear = true;
	optimal_clear.Format = in_format;
	optimal_clear.DepthStencil.Depth = in_depth;
	optimal_clear.DepthStencil.Stencil = in_stencil;
}


void dx_resource::create( cook const& in_cook )
{
	DX12_CHECK_RESULT(
		g_dx.device( )->CreateCommittedResource(
			&in_cook.heap_props,
			in_cook.heap_flags,
			&in_cook.desc,
			in_cook.initial_state,
			in_cook.has_optimal_clear ? &in_cook.optimal_clear : nullptr,
			IID_PPV_ARGS( &m_resource )
		)
	);
}

void dx_resource::destroy( )
{
	s32 const ref_cnt = m_resource->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render