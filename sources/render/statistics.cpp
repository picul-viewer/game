#include "statistics.h"
#include "api.h"
#include "render.h"

#include <macros.h>
#include <lib/allocator.h>
#include <lib/strings.h>
#include <lib/memory.h>

namespace render {

void statistics::create( )
{
	m_event_set.create( m_event_set_memory );

	m_heap.create( max_events * max_frame_delay * 2, D3D12_QUERY_HEAP_TYPE_TIMESTAMP );

	dx_resource::cook cook;
	cook.create_buffer( sizeof(u64) * 2 * max_events * max_frame_delay, false, false, false, false );
	cook.set_heap_type( D3D12_HEAP_TYPE_READBACK );
	cook.set_initial_state( D3D12_RESOURCE_STATE_COPY_DEST );
	m_result_buffer.create( cook );

	u64 freq;
	g_dx.queue_graphics( )->GetTimestampFrequency( &freq );
	m_inv_frequency = 1000.0 / freq;

	memory::set( m_stack, sizeof(m_stack), 0 );

	m_root = nullptr;

	m_query_count = 0;
	m_stack_index = 0;

	m_frame_ticker.tick( );
	m_current_cpu_frame_time = 0.0;
}

void statistics::destroy( )
{
	m_result_buffer.destroy( );
	m_heap.destroy( );
	m_event_set.destroy( );
}

void statistics::resolve( dx_command_list const& in_list, u32 const in_frame_index )
{
	in_list->ResolveQueryData( m_heap, D3D12_QUERY_TYPE_TIMESTAMP, 2 * in_frame_index * max_events, 2 * m_query_count, m_result_buffer, sizeof(u64) * 2 * in_frame_index * max_events );
}

static const float adaptation_speed = 0.05f;

void statistics::process( pstr const out_string, uptr const in_max_chars )
{
	lib::text_writer w( out_string, in_max_chars );

	float elapsed_time = (float)m_frame_ticker.tick( );
	m_current_cpu_frame_time = math::lerp( elapsed_time, m_current_cpu_frame_time, adaptation_speed );

	w.write_mask( TEXT_WRITER_MASK( "FPS: %4.1f\nCPU frame time: %4.3f ms\n" ), 1.0f / m_current_cpu_frame_time, 1000.0f * m_current_cpu_frame_time );

	u32 const frame_index = ( g_render.frame_index( ) + max_frame_delay - 1 ) % max_frame_delay;
	D3D12_RANGE const range = { sizeof(u64) * 2 * max_events * frame_index, sizeof(u64) * 2 * ( max_events * frame_index + m_query_count ) };
	u64* query_result = nullptr;

	DX12_CHECK_RESULT( m_result_buffer->Map( 0, &range, (pvoid*)&query_result ) );

	query_result += 2 * max_events * frame_index;

	event_data* data = m_root;
	while ( data )
	{
		process_event_data( data, w, query_result, 0 );
		data = data->m_next;
	}

	w.write_str( "\0" );

	m_result_buffer->Unmap( 0, nullptr );
}

void statistics::process_event_data( event_data* const in_data, lib::text_writer& in_writer, u64* const in_query_result, u32 const in_depth )
{
	u64 const begin_tick = in_query_result[in_data->m_query_index * 2 + 0];
	u64 const end_tick = in_query_result[in_data->m_query_index * 2 + 1];
	float const elapsed_time = (float)( (double)( end_tick - begin_tick ) * m_inv_frequency );

	in_data->m_current_value = math::lerp( in_data->m_current_value, elapsed_time, adaptation_speed );

	s32 const max_caption_width = 30;
	s32 const pre_identation = 2 * in_depth;
	s32 const post_identation = max_caption_width - pre_identation - (u32)strings::length( in_data->m_name );
	ASSERT_CMP( post_identation, >=, 1 );

	if ( pre_identation == 0 )
		in_writer.write_mask( TEXT_WRITER_MASK( "%s%*c: %4.3f ms\n" ), in_data->m_name, post_identation, ' ', in_data->m_current_value );
	else
		in_writer.write_mask( TEXT_WRITER_MASK( "%*c%s%*c: %4.3f ms\n" ), pre_identation, ' ', in_data->m_name, post_identation, ' ', in_data->m_current_value );

	event_data* child = in_data->m_child;
	while ( child )
	{
		process_event_data( child, in_writer, in_query_result, in_depth + 1 );
		child = child->m_next;
	}
}

void statistics::begin_profile_event( dx_command_list const& in_list, u32 const in_frame_index, pcstr const in_name )
{
	ASSERT_CMP( in_frame_index, <, max_frame_delay );

	event_data* found_data;

	bool const found = m_event_set.find_if_or_insert(
		lib::hash( in_name ),
		[in_name]( event_data const& in_data )
		{
			return in_data.m_name == in_name;
		},
		[in_name, this]( )
		{
			ASSERT( m_query_count < max_events );

			event_data data;
			data.m_name = in_name;
			data.m_child = nullptr;
			data.m_next = nullptr;
			data.m_query_index = m_query_count++;
			data.m_current_value = 0.0f;

			return data;
		},
		found_data
	);

	if ( !found )
	{
		if ( m_stack[m_stack_index] != nullptr )
			m_stack[m_stack_index]->m_next = found_data;
		else if (m_stack_index != 0)
			m_stack[m_stack_index - 1]->m_child = found_data;
		else
			m_root = found_data;
	}

	m_stack[m_stack_index++] = found_data;

	u32 const event_index = ( in_frame_index * max_events + found_data->m_query_index ) * 2 + 0;
	in_list->EndQuery( m_heap, D3D12_QUERY_TYPE_TIMESTAMP, event_index );
}

void statistics::end_profile_event( dx_command_list const& in_list, u32 const in_frame_index )
{
	u32 const event_index = ( in_frame_index * max_events + m_stack[--m_stack_index]->m_query_index ) * 2 + 1;
	in_list->EndQuery( m_heap, D3D12_QUERY_TYPE_TIMESTAMP, event_index );
}

/*statistics::debug_event::debug_event( LPCWSTR const in_name_wstr )
{
	g_statistics.begin_debug_event( in_name_wstr );
}

statistics::debug_event::~debug_event( )
{
	g_statistics.end_debug_event( );
}*/

statistics::profile_event::profile_event( dx_command_list const& in_list, u32 const in_frame_index, pcstr const in_name ) :
	m_list( in_list ),
	m_frame_index( in_frame_index )
{
	g_statistics.begin_profile_event( in_list, in_frame_index, in_name );
}

statistics::profile_event::~profile_event( )
{
	g_statistics.end_profile_event( m_list, m_frame_index );
}

#ifdef USE_RENDER_PROFILING
statistics g_statistics;
#endif // #ifdef USE_RENDER_PROFILING

} // namespace render