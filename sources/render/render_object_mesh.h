#ifndef GUARD_RENDER_RENDER_OBJECT_MESH_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_MESH_H_INCLUDED

#include <types.h>

#include <lib/buffer_array.h>
#include <lib/reader.h>
#include <math/aabb.h>
#include <math/bvh.h>
#include <resource_system/resource_system_types.h>

#include "render_object.h"
#include "model_mesh.h"

namespace render {

class render_object_mesh : public render_object
{
public:
	void initialize( lib::reader& in_reader, lib::buffer_array<task_info>& in_queries );
	void set_queried_resources( queried_resources& in_queried );

	void destroy( );

	void update( math::float4x3 const& in_transform );

	void render( ) const;

	inline model_mesh_handle get_model( ) const { return m_model; }

	inline math::bvh::node_handle get_node_handle( ) const { return m_node_handle; }
	inline void set_node_handle( math::bvh::node_handle const in_handle ) { m_node_handle = in_handle; }

	inline math::float4x3 const& get_transform( ) const { return m_transform; }
	inline math::aabb_aligned const& get_aabb( ) const { return m_aabb; }

private:
	model_mesh_handle		m_model;
	math::bvh::node_handle	m_node_handle;
	u16						m_padding[2];

	// This is 16-byte aligned
	math::float4x3			m_local_transform;
	math::float4x3			m_transform;
	math::aabb_aligned		m_aabb;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_MESH_H_INCLUDED