#ifndef GUARD_RENDER_RENDER_OBJECT_MESH_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_MESH_H_INCLUDED

#include <types.h>

#include <lib/buffer_array.h>
#include <lib/linear_allocator.h>
#include <lib/reader.h>
#include <math/aabb.h>
#include <math/bvh.h>
#include <resource_system/resource_system_types.h>
#include "gpu_uploader.h"

#include "model_mesh.h"

namespace render {

class render_object_mesh
{
public:
	void create( lib::reader& in_reader, lib::buffer_array<task_info>& in_queries );
	void on_resources_ready( queried_resources& in_queried, lib::buffer_array<gpu_upload_task>& in_tasks, linear_allocator& in_allocator, bool const in_is_static );

	void destroy( );

	void update( math::float4x3 const& in_transform );

	inline model_mesh_handle get_model( ) const { return m_model; }

	inline math::bvh::node_handle get_node_handle( ) const { return m_node_handle; }
	inline void set_node_handle( math::bvh::node_handle const in_handle ) { m_node_handle = in_handle; }

	inline math::aabb_aligned const& get_aabb( ) const { return m_aabb; }

private:
	friend class render;

private:
	math::float4x3			m_local_transform;
	math::aabb_aligned		m_aabb;
	u32						m_transform_handle;
	u32						m_object_handle;
	model_mesh_handle		m_model;
	math::bvh::node_handle	m_node_handle;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_MESH_H_INCLUDED