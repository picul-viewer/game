#ifndef GUARD_RENDER_RENDER_OBJECT_POINT_LIGHT_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_POINT_LIGHT_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <lib/linear_allocator.h>
#include <lib/reader.h>
#include <math/matrix.h>
#include "gpu_structures.h"
#include "gpu_uploader.h"

namespace render {

class render_object_point_light
{
public:
	void create( lib::reader& in_reader, lib::buffer_array<task_info>& );
	void on_resources_ready( queried_resources&, lib::buffer_array<gpu_upload_task>& in_tasks, linear_allocator&, bool const in_is_static );

	void destroy( );

	void update( math::float4x3 const& in_transform );
	void set_color( math::float3 const& in_color );

private:
	friend class render;

private:
	gpu::point_light_object	m_data;
	math::float3			m_local_position;
	u32						m_object_handle;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_POINT_LIGHT_H_INCLUDED