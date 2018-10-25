#ifdef RENDER_ENABLE_GPU_PROFILING

#include "profiler.h"
#include "api.h"

#include <macros.h>
#include <lib/strings.h>
#include <lib/memory.h>

namespace render {

profiler::frame_struct::frame_struct( ) :
	m_query( nullptr ),
	m_events_begin( 0 ),
	m_events_end( 0 )
{ }

profiler::event_struct::event_struct( ) :
	m_name( nullptr ),
	m_next_index( 0 ),
	m_t0( nullptr ),
	m_t1( nullptr )
{ }

profiler::profiler( )
{ }

void profiler::create( )
{
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	desc.MiscFlags = 0;

	for ( u32 i = 0; i < max_frames; ++i )
		g_api.get_device( )->CreateQuery( &desc, &m_frames[i].m_query );
	
	desc.Query = D3D11_QUERY_TIMESTAMP;
	
	for ( u32 i = 0; i < max_events; ++i )
	{
		g_api.get_device( )->CreateQuery( &desc, &m_events[i].m_t0 );
		g_api.get_device( )->CreateQuery( &desc, &m_events[i].m_t1 );
	}
}

void profiler::destroy( )
{
	for ( u32 i = 0; i < max_frames; ++i )
	{
		ASSERT( m_frames[i].m_query );
		m_frames[i].m_query->Release( );
	}
	
	for ( u32 i = 0; i < max_events; ++i )
	{
		ASSERT( m_events[i].m_t0 );
		ASSERT( m_events[i].m_t1 );
		m_events[i].m_t0->Release( );
		m_events[i].m_t1->Release( );
	}
}

void profiler::begin_frame( )
{
	ASSERT( m_frames[m_frame_index].m_query );

	g_api.get_context( )->Begin( m_frames[m_frame_index].m_query );
	m_frames[m_frame_index].m_events_begin = m_event_index;
}

void profiler::end_frame( )
{
	ASSERT( m_frames[m_frame_index].m_query );

	g_api.get_context( )->End( m_frames[m_frame_index].m_query );
	m_frames[m_frame_index].m_events_end = m_event_index;

	m_frame_index = ( m_frame_index + 1 ) % max_frames;
}

void profiler::begin_event( pcstr in_name )
{
	ASSERT( m_events[m_event_index].m_t0 );

	m_events[m_event_index].m_name = in_name;
	g_api.get_context( )->End( m_events[m_event_index].m_t0 );

	m_event_index = ( m_event_index + 1 ) % max_events;
}

void profiler::end_event( )
{
	ASSERT( m_events[m_event_index].m_t1 );

	g_api.get_context( )->End( m_events[m_event_index].m_t1 );
	m_events[m_event_index].m_next_index = m_event_index;
}

void profiler::output_statistics( uptr in_delay, uptr in_captions_width, uptr in_offset_size_in_spaces, pstr out_string, uptr in_max_output_size )
{
	uptr const frame_index = m_frame_index - in_delay;

	frame_struct const& frame = m_frames[frame_index];
	
	float period = 0.0f;

	{
		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT frame_data;
		HRESULT result = g_api.get_context( )->GetData( frame.m_query, &frame_data, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0 );
		ASSERT( result != S_FALSE );

		if ( frame_data.Disjoint )
		{
			*out_string = '\0';
			return;
		}

		period = 1000.0f / (float)frame_data.Frequency;
	}

	uptr left_offset = 0;
	uptr index = 0;

	for ( uptr i = frame.m_events_begin; i < frame.m_events_end; i = ( i + 1 ) % max_events )
	{
		ASSERT( index + in_captions_width < in_max_output_size );

		uptr l = strings::length( m_events[i].m_name );
		ASSERT( left_offset + l < in_captions_width );
		uptr right_offset = in_captions_width - ( left_offset + l );

		memory::set( out_string, left_offset, ' ' );
		out_string += left_offset;

		memory::copy( out_string, m_events[i].m_name, l );
		out_string += l;

		memory::set( out_string, right_offset, ' ' );
		out_string += right_offset;

		UINT64 t0, t1;
		{
			HRESULT result = g_api.get_context( )->GetData( m_events[i].m_t0, &t0, sizeof(UINT64), 0 );
			ASSERT( result != S_FALSE );
		}
		{
			HRESULT result = g_api.get_context( )->GetData( m_events[i].m_t1, &t1, sizeof(UINT64), 0 );
			ASSERT( result != S_FALSE );
		}

		uptr const number_length = 10;
		ASSERT( index + number_length < in_max_output_size );

		float time = (float)( t1 - t0 ) * period;

		sprintf( out_string, "%3.2f ms\n", time );
	}
}


profiler g_profiler;


profiler::profiler_frame::profiler_frame( )
{
	g_profiler.begin_frame( );
}

profiler::profiler_frame::~profiler_frame( )
{
	g_profiler.end_frame( );
}

profiler::profiler_event::profiler_event( pcstr in_name )
{
	g_profiler.begin_event( in_name );
}

profiler::profiler_event::~profiler_event( )
{
	g_profiler.end_event( );
}

} // namespace render

#endif // #ifdef RENDER_ENABLE_GPU_PROFILING