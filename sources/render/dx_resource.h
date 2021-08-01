#ifndef GUARD_RENDER_DX_RESOURCE_H_INCLUDED
#define GUARD_RENDER_DX_RESOURCE_H_INCLUDED

#include <types.h>
#include <math/vector.h>
#include "dx12.h"
#include "dx_heap.h"

namespace render {

class dx_resource
{
public:
	struct cook
	{
		void create_buffer(
			u64 const in_size,
			bool const in_allow_shader_resource,
			bool const in_allow_unordered_access,
			bool const in_allow_render_target,
			bool const in_allow_simultaneous_access = false
		);
		void create_texture1d(
			DXGI_FORMAT const in_format,
			u32 const in_width,
			u32 const in_array_size,
			u32 const in_mip_levels,
			bool const in_allow_shader_resource,
			bool const in_allow_unordered_access,
			bool const in_allow_render_target,
			bool const in_allow_depth_stencil,
			bool const in_allow_simultaneous_access
		);
		void create_texture2d(
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
		);
		void create_texture3d(
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
		);

		void set_heap_type( D3D12_HEAP_TYPE const in_heap_type );
		void set_initial_state( D3D12_RESOURCE_STATES const in_state );
		void set_clear_value( DXGI_FORMAT const in_format, math::float4 const in_value );
		void set_clear_value( DXGI_FORMAT const in_format, float const in_depth, u8 const in_stencil );

		D3D12_HEAP_PROPERTIES heap_props;
		D3D12_HEAP_FLAGS heap_flags;
		D3D12_RESOURCE_DESC desc;
		D3D12_RESOURCE_STATES initial_state;
		D3D12_CLEAR_VALUE optimal_clear;
		bool has_optimal_clear = false;
	};

	dx_resource( ) = default;
	dx_resource( ID3D12Resource* in_resource );

	void create( cook const& in_cook );
	void destroy( );

	inline void set( ID3D12Resource* const in_resource ) { m_resource = in_resource; }

	inline operator ID3D12Resource*( ) const { return m_resource; }
	inline ID3D12Resource* operator->( ) const { return m_resource; }

private:
	ID3D12Resource* m_resource;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_RESOURCE_H_INCLUDED