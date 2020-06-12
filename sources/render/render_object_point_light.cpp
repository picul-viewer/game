#include "render_object_point_light.h"
#include "resources.h"
#include <math/math_3d.h>

namespace render {

void render_object_point_light::create( lib::reader& in_reader, lib::buffer_array<task_info>& )
{
	ASSERT( aligned( this, 16 ) );

	m_local_position = in_reader.read<math::float3>( );
	m_data.range = in_reader.read<float>( );
	m_data.squared_range = m_data.range * m_data.range;
	m_data.color = in_reader.read<math::float3>( );
}

void render_object_point_light::on_resources_ready( queried_resources&, lib::buffer_array<gpu_upload_task>& in_tasks, linear_allocator&, bool const in_is_static )
{
	if ( in_is_static )
	{
		m_data.position = m_local_position;
		m_object_handle = g_resources.point_lights( ).allocate_static( );
		g_resources.point_lights( ).get_init_tasks( m_object_handle, m_data, in_tasks );
	}
	else
	{
		m_object_handle = g_resources.point_lights( ).allocate_dynamic( );
		update( math::float4x3::identity( ) );
	}
}

void render_object_point_light::destroy( )
{
	g_resources.point_lights( ).deallocate( m_object_handle );
}

void render_object_point_light::update( math::float4x3 const& in_transform )
{
	m_data.position = math::modify_by_transform( m_local_position, in_transform );
	g_resources.point_lights( ).update( m_object_handle, m_data );
}

void render_object_point_light::set_color( math::float3 const& in_color )
{
	m_data.color = in_color;
	g_resources.point_lights( ).update( m_object_handle, m_data );
}

} // namespace render