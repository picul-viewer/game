#include "gpu_resource.h"
#include "dx.h"

namespace render {

void gpu_resource::cook::set_buffer(
	u64 const in_size,
	bool const in_allow_rtv,
	bool const in_allow_srv,
	bool const in_allow_uav
)
{
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Width = in_size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	desc.Flags =
		( in_allow_rtv ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_uav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void gpu_resource::cook::set_texture_1d(
	DXGI_FORMAT const in_format,
	u64 const in_width,
	u32 const in_array_size,
	u32 const in_mip_levels,
	bool const in_allow_rtv,
	bool const in_allow_dsv,
	bool const in_allow_srv,
	bool const in_allow_uav
)
{
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Width = in_width;
	desc.Height = 1;
	desc.DepthOrArraySize = in_array_size;
	desc.MipLevels = in_mip_levels;
	desc.Format = in_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	ASSERT( !in_allow_dsv || ( !in_allow_rtv && !in_allow_uav ) );

	desc.Flags =
		( in_allow_rtv ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_dsv ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE ) |
		( ( in_allow_dsv && !in_allow_srv ) ? D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_uav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void gpu_resource::cook::set_texture_2d(
	DXGI_FORMAT const in_format,
	u64 const in_width,
	u32 const in_height,
	u32 const in_array_size,
	u32 const in_mip_levels,
	bool const in_allow_rtv,
	bool const in_allow_dsv,
	bool const in_allow_srv,
	bool const in_allow_uav
)
{
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Width = in_width;
	desc.Height = in_height;
	desc.DepthOrArraySize = in_array_size;
	desc.MipLevels = in_mip_levels;
	desc.Format = in_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	ASSERT( !in_allow_dsv || ( !in_allow_rtv && !in_allow_uav ) );

	desc.Flags =
		( in_allow_rtv ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_dsv ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE ) |
		( ( in_allow_dsv && !in_allow_srv ) ? D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_uav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

void gpu_resource::cook::set_texture_3d(
	DXGI_FORMAT const in_format,
	u64 const in_width,
	u32 const in_height,
	u32 const in_depth,
	u32 const in_mip_levels,
	bool const in_allow_rtv,
	bool const in_allow_dsv,
	bool const in_allow_srv,
	bool const in_allow_uav
)
{
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Width = in_width;
	desc.Height = in_height;
	desc.DepthOrArraySize = in_depth;
	desc.MipLevels = in_mip_levels;
	desc.Format = in_format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	ASSERT( !in_allow_dsv || ( !in_allow_rtv && !in_allow_uav ) );

	desc.Flags =
		( in_allow_rtv ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_dsv ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE ) |
		( ( in_allow_dsv && !in_allow_srv ) ? D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE : D3D12_RESOURCE_FLAG_NONE ) |
		( in_allow_uav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE );
}

D3D12_RESOURCE_ALLOCATION_INFO gpu_resource::cook::get_allocation_info( ) const
{
	return g_dx.device( )->GetResourceAllocationInfo( 0, 1, &desc );
}

void gpu_resource::cook::set_resource_memory(
	ID3D12Heap* const in_heap,
	u64 const in_heap_offset
)
{
	heap = in_heap;
	heap_offset = in_heap_offset;
}

void gpu_resource::cook::set_initial_state(
	D3D12_RESOURCE_STATES const in_state
)
{
	initial_state = in_state;
}

void gpu_resource::cook::set_color_clear_value(
	math::float4 const& in_color
)
{
	clear_value.Format = desc.Format;
	clear_value.Color[0] = in_color.x;
	clear_value.Color[1] = in_color.y;
	clear_value.Color[2] = in_color.z;
	clear_value.Color[3] = in_color.w;
}

void gpu_resource::cook::set_depth_clear_value(
	float const in_depth,
	u8 const in_stencil
)
{
	clear_value.Format = desc.Format;
	clear_value.DepthStencil.Depth = in_depth;
	clear_value.DepthStencil.Stencil = in_stencil;
}

void gpu_resource::create( cook const& in_cook )
{
	if ( in_cook.heap != nullptr )
	{
		DX12_CHECK_RESULT(
			g_dx.device( )->CreatePlacedResource(
				in_cook.heap,
				in_cook.heap_offset,
				&in_cook.desc,
				in_cook.initial_state,
				&in_cook.clear_value,
				IID_PPV_ARGS( &m_resource )
			)
		);
	}
	else
	{
		// Workaround.

		D3D12_HEAP_PROPERTIES heap_props;
		heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.CreationNodeMask = 0;
		heap_props.VisibleNodeMask = 0;

		D3D12_HEAP_FLAGS const heap_flags =
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES |
			( ( in_cook.desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ) ? D3D12_HEAP_FLAG_DENY_BUFFERS : D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES ) |
			( ( in_cook.desc.Flags & ( D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL ) ) ? D3D12_HEAP_FLAG_NONE : D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES );
									   
		DX12_CHECK_RESULT(
			g_dx.device( )->CreateCommittedResource(
				&heap_props,
				heap_flags,
				&in_cook.desc,
				in_cook.initial_state,
				&in_cook.clear_value,
				IID_PPV_ARGS( &m_resource )
			)
		);
	}
}

void gpu_resource::destroy( )
{
	m_resource->Release( );
}

} // namespace render