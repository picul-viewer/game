#ifndef GUARD_RENDER_RESOURCES_H_INCLUDED
#define GUARD_RENDER_RESOURCES_H_INCLUDED

#include <types.h>

#include <math/matrix.h>

#include "dx.h"

#include "gpu_double_pool_allocator.h"
#include "gpu_heap_allocator.h"
#include "gpu_pool_allocator.h"
#include "gpu_uploader.h"

#include "dx_resource.h"
#include "dx_descriptor_heap.h"

#include "render_allocator.h"
#include "ui_batch.h"

namespace render {

enum images
{
	image_v_buffer_polygon_id = 0,

	image_count
};

enum image_srvs
{
	image_srv_v_buffer_polygon_id = 0,
	image_srv_depth_buffer,

	image_srv_count
};

enum image_rtvs
{
	image_rtv_v_buffer_polygon_id = 0,

	image_rtv_output_0,
	image_rtv_output_1,

	image_rtv_count
};

class resources
{
public:
	enum : u32 {
		texture_descriptors_count = 256,
		mesh_objects_count = 1024,
		transform_count = 16384
	};

	enum : u32 {
		hlsl_images_space = 8,
		hlsl_textures_space = 9
	};

public:
	void create( );
	void destroy( );
	

	u32 create_texture( dx_resource const in_texture, D3D12_SHADER_RESOURCE_VIEW_DESC const& in_srv_desc );
	void destroy_texture( u32 const in_index );

	u32 create_mesh_index_buffer( u32 const in_index_count );
	u32 create_mesh_vertex_buffer( u32 const in_vertex_count );

	void destroy_mesh_index_buffer( u32 const in_index_offset, u32 const in_index_count );
	void destroy_mesh_vertex_buffer( u32 const in_vertex_offset, u32 const in_vertex_count );

	u32 create_mesh_object( );
	void destroy_mesh_object( u32 const in_index );

	u32 create_static_transform( );
	u32 create_dynamic_transform( );
	void destroy_transform( u32 const in_handle );
	void get_transform_init_tasks( u32 const in_handle, math::float4x3& data, lib::buffer_array<gpu_upload_task>& in_tasks );
	void update_dynamic_transform( u32 const in_handle, math::float4x3 const& data );
	bool need_dynamic_transforms_update( ) const;
	void update_dynamic_transforms_task( gpu_upload_task& in_task );


	dx_descriptor_heap const& srv_heap( ) const { return m_srv_heap; }
	dx_descriptor_heap const& rtv_heap( ) const { return m_rtv_heap; }
	inline dx_resource const& depth_buffer( ) const { return m_depth_buffer; }
	inline dx_resource const& index_buffer( ) const { return m_index_buffer; }
	inline dx_resource const& vertex_buffer( ) const { return m_vertex_buffer; }
	inline dx_resource const& vertex_data_buffer( ) const { return m_vertex_data_buffer; }
	inline dx_resource const& mesh_object_buffer( ) const { return m_mesh_object_buffer; }
	inline dx_resource const& constant_buffer( u32 const in_index ) const { ASSERT_CMP( in_index, <, max_frame_delay ); return m_constant_buffers[in_index]; }
	inline dx_resource const& transform_buffer( u32 const in_index ) const { ASSERT_CMP( in_index, <, max_frame_delay ); return m_transform_buffer[in_index]; }

	D3D12_INDEX_BUFFER_VIEW index_buffer_view( ) const;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view( ) const;
	D3D12_VERTEX_BUFFER_VIEW vertex_data_buffer_view( ) const;

	dx_resource const& image( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE srv( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE rtv( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv( ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE read_only_dsv( ) const;

	inline render_allocator& render_allocator( ) { return m_render_allocator; }
	inline ui_batch& ui_batch( ) { return m_ui_batch; }

private:
	void create_resources( );
	void destroy_resources( );

	void create_images( );
	void destroy_images( );

private:
	gpu_pool_allocator m_texture_descriptor_allocator;
	gpu_heap_allocator m_mesh_index_allocator;
	gpu_heap_allocator m_mesh_vertex_allocator;
	gpu_pool_allocator m_mesh_object_allocator;
	gpu_double_pool_allocator m_transform_allocator;
	math::float4x3* m_cpu_transform_data;
	math::float4x3* m_cpu_transform_data_staging;

	dx_resource m_images[image_count];
	dx_resource m_depth_buffer;
	dx_resource m_index_buffer;
	dx_resource m_vertex_buffer;
	dx_resource m_vertex_data_buffer;
	dx_resource m_mesh_object_buffer;
	dx_resource m_constant_buffers[max_frame_delay];
	dx_resource m_transform_buffer[max_frame_delay];
	dx_descriptor_heap m_srv_heap;
	dx_descriptor_heap m_rtv_heap;
	dx_descriptor_heap m_dsv_heap;

	::render::render_allocator m_render_allocator;
	::render::ui_batch m_ui_batch;

	pvoid m_allocator_memory;

	// Current and previuos
	math::u32x4 m_transform_update_bounds;

};

extern resources g_resources;

} // namespace render

#endif // #ifndef GUARD_RENDER_RESOURCES_H_INCLUDED