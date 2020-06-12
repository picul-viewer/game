#include "ui_processor.h"
#include <lib/fixed_string.h>
#include <lib/algorithms.h>
#include "parameters.h"
#include "render.h"
#include "texture.h"

namespace render {

void ui_processor::create( )
{
	dx_resource::cook command_cook;
	command_cook.create_buffer(
		max_batches * sizeof(gpu::draw_indirect_command),
		false, false, false, false
	);
	command_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
	command_cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

	dx_resource::cook vertex_cook;
	vertex_cook.create_buffer(
		max_quads * 6 * sizeof(vertex_data),
		false, false, false, false
	);
	vertex_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
	vertex_cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

	for ( u32 i = 0; i < max_frame_delay; ++i )
	{
		m_ui_command_buffer[i].create( command_cook );
		set_dx_name( m_ui_command_buffer[i], format( "ui_command_buffer #%d", i ) );

		m_ui_vertex_buffer[i].create( vertex_cook );
		set_dx_name( m_ui_vertex_buffer[i], format( "ui_vertex_buffer #%d", i ) );
	}

	m_current_batch_count = 0;
	m_current_quad_count = 0;
	m_batched_quad_count = 0;
}

void ui_processor::destroy( )
{
	for ( u32 i = 0; i < max_frame_delay; ++i )
	{
		m_ui_command_buffer[i].destroy( );
		m_ui_vertex_buffer[i].destroy( );
	}
}

void ui_processor::add_quad(
	math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
	math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture )
{
	quad_data& data			= m_quad_data[m_current_quad_count++];
	ASSERT_CMP				( m_current_quad_count, <, max_quads );

	data.corners_position	= in_corners_position;
	data.corners_texcoord	= in_corners_texcoord;
	data.mult_color			= in_mult_color;
	data.add_color			= in_add_color;
	data.texture			= in_texture;
}

void ui_processor::add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color )
{
	quad_data& data			= m_quad_data[m_current_quad_count++];

	data.corners_position	= in_corners_position;
	data.corners_texcoord	= math::u16x4( 0, 0, 0, 0 );
	data.mult_color			= math::half4( 0.0f );
	data.add_color			= in_color;
	data.texture			= texture_handle::invalid;
}

void ui_processor::next_level( )
{
	m_batch_data[m_current_batch_count++] = m_current_quad_count - m_batched_quad_count;
	m_batched_quad_count = m_current_quad_count;
}

u32 ui_processor::upload_task_count_needed( ) const
{
	return m_current_quad_count != 0 ? 2 : 0;
}

u32 ui_processor::prepare_ui( lib::buffer_array<gpu_upload_task>& in_tasks )
{
	ASSERT_CMP( m_current_quad_count, ==, m_batched_quad_count );

	if ( m_current_batch_count == 0 )
		return 0;

	math::u32x4 const screen_dimensions = math::u16x4( g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, g_parameters.screen_resolution.x, g_parameters.screen_resolution.y );

	vertex_data* v = m_cpu_vertex_buffer;

	u32 first_quad_index = 0;
	u32 command_count = 0;

	for ( u32 j = 0; j < m_current_batch_count; ++j )
	{
		u32 const quad_count = m_batch_data[j];
		if ( quad_count == 0 )
			continue;

		quad_data* begin = m_quad_data + first_quad_index;
		quad_data* end = begin + quad_count;

		auto const comparator = []( quad_data const& l, quad_data const& r )
		{
			return l.texture < r.texture;
		};

		lib::sort( begin, end, comparator );

		m_cpu_command_buffer[command_count].vertex_count_per_instance	= 6 * quad_count;
		m_cpu_command_buffer[command_count].instance_count				= 1;
		m_cpu_command_buffer[command_count].start_vertex_location		= 6 * first_quad_index;
		m_cpu_command_buffer[command_count].start_instance_location		= command_count;

		first_quad_index += quad_count;
		++command_count;

		for ( quad_data* i = begin; i != end; ++i )
		{
			math::s16x4 normalized_position = math::u16x4( math::u32x4( i->corners_position ) * math::u32x4( 65535 ) / screen_dimensions ) - math::u16x4( 32768 );
			normalized_position.y = 65535 - normalized_position.y;
			normalized_position.w = 65535 - normalized_position.w;

			u32 const texture_id = (i->texture == texture_handle::invalid) ? 0xFFFFFFFF : texture::from_handle( i->texture )->get( );

			v->position = math::s16x2( normalized_position.x, normalized_position.y );
			v->texcoord = math::u16x2( i->corners_texcoord.x, i->corners_texcoord.y );
			v->mult_color = i->mult_color;
			v->add_color = i->add_color;
			v->texture_id = texture_id;
			++v;

			v->position = math::s16x2( normalized_position.x, normalized_position.w );
			v->texcoord = math::u16x2( i->corners_texcoord.x, i->corners_texcoord.w );
			v->mult_color = i->mult_color;
			v->add_color = i->add_color;
			v->texture_id = texture_id;
			++v;

			v->position = math::s16x2( normalized_position.z, normalized_position.y );
			v->texcoord = math::u16x2( i->corners_texcoord.z, i->corners_texcoord.y );
			v->mult_color = i->mult_color;
			v->add_color = i->add_color;
			v->texture_id = texture_id;
			++v;

			v->position = math::s16x2( normalized_position.x, normalized_position.w );
			v->texcoord = math::u16x2( i->corners_texcoord.x, i->corners_texcoord.w );
			v->mult_color = i->mult_color;
			v->add_color = i->add_color;
			v->texture_id = texture_id;
			++v;

			v->position = math::s16x2( normalized_position.z, normalized_position.y );
			v->texcoord = math::u16x2( i->corners_texcoord.z, i->corners_texcoord.y );
			v->mult_color = i->mult_color;
			v->add_color = i->add_color;
			v->texture_id = texture_id;
			++v;

			v->position = math::s16x2( normalized_position.z, normalized_position.w );
			v->texcoord = math::u16x2( i->corners_texcoord.z, i->corners_texcoord.w );
			v->mult_color = i->mult_color;
			v->add_color = i->add_color;
			v->texture_id = texture_id;
			++v;
		}
	}

	ASSERT_CMP( first_quad_index, ==, m_current_quad_count );

	gpu_upload_task& commands_task = in_tasks.emplace_back( );
	commands_task.set_source_data( m_cpu_command_buffer, command_count * sizeof(gpu::draw_indirect_command) );
	commands_task.set_buffer_upload( m_ui_command_buffer[g_render.frame_index( ) % max_frame_delay], 0 );

	gpu_upload_task& vertices_task = in_tasks.emplace_back( );
	vertices_task.set_source_data( m_cpu_vertex_buffer, m_current_quad_count * 6 * sizeof(vertex_data) );
	vertices_task.set_buffer_upload( m_ui_vertex_buffer[g_render.frame_index( ) % max_frame_delay], 0 );

	m_current_batch_count = 0;
	m_current_quad_count = 0;
	m_batched_quad_count = 0;

	return command_count;
}

D3D12_VERTEX_BUFFER_VIEW ui_processor::vertex_buffer_view( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, max_frame_delay );

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = m_ui_vertex_buffer[in_index]->GetGPUVirtualAddress( );
	vbv.SizeInBytes = max_quads * 6 * sizeof(vertex_data);
	vbv.StrideInBytes = sizeof(vertex_data);
	return vbv;
}

} // namespace render