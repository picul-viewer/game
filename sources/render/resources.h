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

#include "render_allocator.h"

namespace render {

enum images
{
	image_v_buffer_polygon_id = 0,
	image_radiance,
	image_depth_buffer,
	image_sun_shadowmap,

	image_count
};

enum image_srv_uavs
{
	image_srv_v_buffer_polygon_id = 0,
	image_srv_depth_buffer,
	image_srv_radiance,
	image_srv_sun_shadowmap,

	image_srv_count,

	image_uav_radiance = image_srv_count,

	image_srv_uav_count,

	image_uav_count = image_srv_uav_count - image_srv_count
};

enum image_rtvs
{
	image_rtv_v_buffer_polygon_id = 0,

	image_rtv_output_0,
	image_rtv_output_1,

	image_rtv_count
};

enum image_dsvs
{
	image_dsv_screen = 0,
	image_dsv_screen_readonly,
	image_dsv_sun_shadowmap,

	image_dsv_count
};

class resources
{
public:
	enum : u32 {
		texture_descriptors_count = 1024,
		mesh_objects_count = 4096,
		transform_count = 16384,
		point_light_count = 4096
	};

	enum : u32 {
		hlsl_images_space = 8,
		hlsl_textures_space = 9
	};

	enum : u32 {
		sun_shadowmap_dimension = 4096
	};

	typedef gpu_object_manager<math::float4x3> transforms_type;
	typedef gpu_object_manager<gpu::point_light_object> point_lights_type;

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


	dx_descriptor_heap const& srv_uav_heap( ) const { return m_srv_uav_heap; }
	dx_descriptor_heap const& rtv_heap( ) const { return m_rtv_heap; }
	inline dx_resource const& index_buffer( ) const { return m_index_buffer; }
	inline dx_resource const& vertex_buffer( ) const { return m_vertex_buffer; }
	inline dx_resource const& vertex_data_buffer( ) const { return m_vertex_data_buffer; }
	inline dx_resource const& mesh_object_buffer( ) const { return m_mesh_object_buffer; }
	inline dx_resource const& constant_buffer( u32 const in_index ) const { ASSERT_CMP( in_index, <, max_frame_delay ); return m_constant_buffers[in_index]; }

	inline transforms_type& transforms( ) { return m_transforms; }
	inline point_lights_type& point_lights( ) { return m_point_lights; }

	D3D12_INDEX_BUFFER_VIEW index_buffer_view( ) const;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view( ) const;
	D3D12_VERTEX_BUFFER_VIEW vertex_data_buffer_view( ) const;

	dx_resource const& image( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE srv( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE rtv( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE uav( u32 const in_index ) const;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv( u32 const in_index ) const;

	inline render_allocator& render_allocator( ) { return m_render_allocator; }

private:
	void create_resources( );
	void destroy_resources( );

	void create_images( );
	void destroy_images( );

private:
	transforms_type m_transforms;
	point_lights_type m_point_lights;

	gpu_pool_allocator m_texture_descriptor_allocator;
	gpu_heap_allocator m_mesh_index_allocator;
	gpu_heap_allocator m_mesh_vertex_allocator;
	gpu_pool_allocator m_mesh_object_allocator;

	dx_resource m_images[image_count];
	dx_resource m_index_buffer;
	dx_resource m_vertex_buffer;
	dx_resource m_vertex_data_buffer;
	dx_resource m_mesh_object_buffer;
	dx_resource m_constant_buffers[max_frame_delay];
	dx_descriptor_heap m_srv_uav_heap;
	dx_descriptor_heap m_rtv_heap;
	dx_descriptor_heap m_dsv_heap;

	::render::render_allocator m_render_allocator;

	pvoid m_allocator_memory;

};

extern resources g_resources;

} // namespace render

#endif // #ifndef GUARD_RENDER_RESOURCES_H_INCLUDED