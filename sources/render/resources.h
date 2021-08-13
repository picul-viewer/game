#ifndef GUARD_RENDER_RESOURCES_H_INCLUDED
#define GUARD_RENDER_RESOURCES_H_INCLUDED

#include <types.h>

#include <math/matrix.h>

#include "dx.h"

#include "gpu_double_pool_allocator.h"
#include "gpu_heap_allocator.h"
#include "gpu_pool_allocator.h"
#include "gpu_object_manager.h"
#include "gpu_uploader.h"

#include "gpu_structures.h"

#include "dx_resource.h"
#include "dx_descriptor_heap.h"
#include "dx_views.h"

#include "render_allocator.h"

namespace render {

class resources
{
public:
	enum : u32 {
		texture_descriptors_count = 2048,
		mesh_objects_count = 4096,
		transform_count = 16384,
		point_light_count = 4096
	};

	enum : u32 {
		hlsl_root_descriptors_space = 0,
		hlsl_table_descriptors_space = 1,
		hlsl_textures_space = 2
	};

	typedef gpu_object_manager<math::float4x3> transforms_type;
	typedef gpu_object_manager<gpu::point_light_object> point_lights_type;

public:
	void create( u32 const in_descriptor_count, u32 const in_rtv_count, u32 const in_dsv_count );
	void destroy( );
	

	u32 create_texture( dx_resource const in_texture, D3D12_SHADER_RESOURCE_VIEW_DESC const& in_srv_desc );
	void destroy_texture( u32 const in_index );

	u32 create_mesh_index_buffer( u32 const in_index_count );
	u32 create_mesh_vertex_buffer( u32 const in_vertex_count );

	void destroy_mesh_index_buffer( u32 const in_index_offset, u32 const in_index_count );
	void destroy_mesh_vertex_buffer( u32 const in_vertex_offset, u32 const in_vertex_count );

	u32 create_mesh_object( );
	void destroy_mesh_object( u32 const in_index );


	dx_descriptor_heap const& descriptor_heap( ) const { return m_descriptor_heap; }
	dx_descriptor_heap const& rtv_heap( ) const { return m_rtv_heap; }
	inline dx_resource const& index_buffer( ) const { return m_index_buffer; }
	inline dx_resource const& vertex_buffer( ) const { return m_vertex_buffer; }
	inline dx_resource const& vertex_data_buffer( ) const { return m_vertex_data_buffer; }
	inline dx_resource const& mesh_object_buffer( ) const { return m_mesh_object_buffer; }

	inline transforms_type& transforms( ) { return m_transforms; }
	inline point_lights_type& point_lights( ) { return m_point_lights; }

	D3D12_INDEX_BUFFER_VIEW index_buffer_view( ) const;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view( ) const;
	D3D12_VERTEX_BUFFER_VIEW vertex_data_buffer_view( ) const;

	inline render_allocator& render_allocator( ) { return m_render_allocator; }

	void reset_descriptors( );

	D3D12_CPU_DESCRIPTOR_HANDLE create_rtv( dx_resource const in_resource, dx_rtv_cook const& in_cook );
	D3D12_CPU_DESCRIPTOR_HANDLE create_dsv( dx_resource const in_resource, dx_dsv_cook const& in_cook );

	u32 allocate_descriptors( u32 const in_descriptor_count );

	void create_cbv( u32 const in_index, dx_cbv_cook const& in_cook );
	void create_srv( u32 const in_index, dx_resource const in_resource, dx_srv_cook const& in_cook );
	void create_uav( u32 const in_index, dx_resource const in_resource, dx_uav_cook const& in_cook );
	void create_uav( u32 const in_index, dx_resource const in_resource, dx_resource const in_counter_buffer, dx_uav_cook const& in_cook );

	D3D12_GPU_DESCRIPTOR_HANDLE get_descriptor_table_handle( u32 const in_table ) const;
	D3D12_GPU_DESCRIPTOR_HANDLE get_common_table_handle( ) const;
	void serialize_common_descriptor_table_binding( dx_root_signature::root_parameter& in_parameter, D3D12_SHADER_VISIBILITY const in_visibility );

private:
	void create_resources( );
	void destroy_resources( );

private:
	transforms_type m_transforms;
	point_lights_type m_point_lights;

	gpu_pool_allocator m_texture_descriptor_allocator;
	gpu_heap_allocator m_mesh_index_allocator;
	gpu_heap_allocator m_mesh_vertex_allocator;
	gpu_pool_allocator m_mesh_object_allocator;

	dx_resource m_index_buffer;
	dx_resource m_vertex_buffer;
	dx_resource m_vertex_data_buffer;
	dx_resource m_mesh_object_buffer;

	dx_descriptor_heap m_descriptor_heap;
	dx_descriptor_heap m_rtv_heap;
	dx_descriptor_heap m_dsv_heap;
	u32 m_descriptor_count;
	u32 m_rtv_count;
	u32 m_dsv_count;
	u32 m_descriptor_offset;
	u32 m_rtv_offset;
	u32 m_dsv_offset;

	dx_root_signature::descriptor_range m_common_descriptor_ranges[1];

	::render::render_allocator m_render_allocator;

	pvoid m_allocator_memory;

};

extern resources g_resources;

} // namespace render

#endif // #ifndef GUARD_RENDER_RESOURCES_H_INCLUDED