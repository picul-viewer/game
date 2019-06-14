#include "statistics.h"
#include "api.h"

#include <macros.h>
#include <lib/allocator.h>
#include <lib/strings.h>
#include <lib/memory.h>
#include <lib/text_writer.h>

#include <system/file.h>

#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )

namespace render {

statistics::frame_struct::frame_struct( ) :
	m_query( nullptr ),
	m_t0( nullptr ),
	m_t1( nullptr ),
	m_events_begin( (u32)-1 ),
	m_events_end( (u32)-1 )
{ }

statistics::event_struct::event_struct( ) :
	m_t0( nullptr ),
	m_t1( nullptr )
{ }

statistics::statistics( )
{ }

void statistics::create( )
{
	m_ticker.tick( );

	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	desc.MiscFlags = 0;

	for ( u32 i = 0; i < max_frames; ++i )
		g_api.get_device( )->CreateQuery( &desc, &m_frames[i].m_query );
	
	desc.Query = D3D11_QUERY_TIMESTAMP;
	
	for ( u32 i = 0; i < max_frames; ++i )
	{
		g_api.get_device( )->CreateQuery( &desc, &m_frames[i].m_t0 );
		g_api.get_device( )->CreateQuery( &desc, &m_frames[i].m_t1 );
	}
	
	for ( u32 i = 0; i < max_events; ++i )
	{
		g_api.get_device( )->CreateQuery( &desc, &m_events[i].m_t0 );
		g_api.get_device( )->CreateQuery( &desc, &m_events[i].m_t1 );
	}
}

void statistics::destroy( )
{
	for ( u32 i = 0; i < max_frames; ++i )
	{
		ASSERT( m_frames[i].m_query );
		ASSERT( m_frames[i].m_t0 );
		ASSERT( m_frames[i].m_t1 );
		m_frames[i].m_query->Release( );
		m_frames[i].m_t0->Release( );
		m_frames[i].m_t1->Release( );
	}
	
	for ( u32 i = 0; i < max_events; ++i )
	{
		ASSERT( m_events[i].m_t0 );
		ASSERT( m_events[i].m_t1 );
		m_events[i].m_t0->Release( );
		m_events[i].m_t1->Release( );
	}
}

void statistics::begin_frame( )
{
	// Update previous frame time.
	float const frame_time = (float)m_ticker.tick( );
	uptr const previous_frame_index = ( m_frame_index - 1 + max_frames ) % max_frames;
	m_frames[previous_frame_index].m_frame_time = frame_time * 1000.0f;

	// GPU queries.
	ASSERT( m_frames[m_frame_index].m_query );
	ASSERT( m_frames[m_frame_index].m_t0 );

	g_api.get_context( )->Begin( m_frames[m_frame_index].m_query );
	g_api.get_context( )->End( m_frames[m_frame_index].m_t0 );

	m_frames[m_frame_index].m_events_begin = m_event_index;
	m_frames[m_frame_index].m_events_end = (u32)-1;
}

void statistics::end_frame( float const in_elapsed_cpu_time )
{
	// CPU render frame time.
	m_frames[m_frame_index].m_render_cpu_frame_time = in_elapsed_cpu_time * 1000.0f;

	// GPU queries.
	ASSERT( m_frames[m_frame_index].m_query );
	ASSERT( m_frames[m_frame_index].m_t1 );

	g_api.get_context( )->End( m_frames[m_frame_index].m_t1 );
	g_api.get_context( )->End( m_frames[m_frame_index].m_query );

	m_frames[m_frame_index].m_events_end = m_event_index;

	m_frame_index = ( m_frame_index + 1 ) % max_frames;
}

void statistics::process_frame( uptr const in_delay )
{
	uptr const frame_index = ( m_frame_index + max_frames - in_delay ) % max_frames;
	frame_struct& frame = m_frames[frame_index];
	
	if ( frame.m_events_end == (u32)-1 )
		return;

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT frame_data;
	g_api.get_context( )->GetData( frame.m_query, &frame_data, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0 );

	float const precision = 1000.0f / (float)frame_data.Frequency;
	frame.m_precision = precision;

	UINT64 frame_t0, frame_t1;
	g_api.get_context( )->GetData( frame.m_t0, &frame_t0, sizeof(UINT64), 0 );
	g_api.get_context( )->GetData( frame.m_t1, &frame_t1, sizeof(UINT64), 0 );

	frame.m_render_gpu_frame_time = (float)( frame_t1 - frame_t0 ) * precision;

	for ( uptr i = frame.m_events_begin; i != frame.m_events_end; i = ( i + 1 ) % max_events )
	{
		event_struct& event = m_events[i];

		UINT64 t0, t1;
		g_api.get_context( )->GetData( event.m_t0, &t0, sizeof(UINT64), 0 );
		g_api.get_context( )->GetData( event.m_t1, &t1, sizeof(UINT64), 0 );
		
		event.m_time_start = (float)( t0 - frame_t0 ) * precision;
		event.m_time_finish = (float)( t1 - frame_t0 ) * precision;
	}
}

void statistics::begin_debug_event( LPCWSTR const in_name_wstr )
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA( 255, 255, 255, 255 ), in_name_wstr );
}

void statistics::end_debug_event( )
{
	D3DPERF_EndEvent( );
}

u32 statistics::begin_profile_event( pcstr const in_name )
{
	ASSERT( m_events[m_event_index].m_t0 );

	m_events[m_event_index].m_name = in_name;
	g_api.get_context( )->End( m_events[m_event_index].m_t0 );

	u32 const result = m_event_index;
	m_event_index = ( m_event_index + 1 ) % max_events;

	return result;
}

void statistics::end_profile_event( u32 const in_event_index )
{
	ASSERT( m_events[in_event_index].m_t1 );

	g_api.get_context( )->End( m_events[in_event_index].m_t1 );
}

void statistics::output_render_statistics( uptr const in_delay, pstr const out_string, uptr const in_max_chars ) const
{
	uptr const frame_index = ( m_frame_index + max_frames - in_delay ) % max_frames;

	frame_struct const& frame = m_frames[frame_index];
	
	if ( frame.m_events_end == (uptr)-1 )
		return;

	lib::text_writer w( out_string, in_max_chars );
	
	w.write_mask( TEXT_WRITER_MASK( "Frametime: %4.3f ms\n" ), frame.m_frame_time );
	w.write_mask( TEXT_WRITER_MASK( "CPU render: %4.3f ms\n" ), frame.m_render_cpu_frame_time );
	w.write_mask( TEXT_WRITER_MASK( "GPU render: %4.3f ms\n" ), frame.m_render_gpu_frame_time );

	for ( uptr i = frame.m_events_begin; i != frame.m_events_end; i = ( i + 1 ) % max_events )
	{
		event_struct const& event = m_events[i];

		w.write_mask(
			TEXT_WRITER_MASK( "%25s: %4.3f ms\n" ),
			event.m_name, event.m_time_finish - event.m_time_start );
	}
}

/*
void statistics::save_render_statistics( uptr const in_delay, pcstr in_file_name ) const
{
	static u32 counter = 0;
	++counter;

	if ( counter % 16 != 0 )
		return;

	uptr const frame_index = ( m_frame_index + max_frames - in_delay ) % max_frames;

	frame_struct const& frame = m_frames[frame_index];
	
	if ( frame.m_events_end == (uptr)-1 )
		return;

	uptr const size = 4 * Mb;
	pstr const data = virtual_allocator( ).allocate( size );

	text_config cfg( data );

	cfg.write_str( "{ \"traceEvents\": [\n" );

	for ( uptr i = frame.m_events_begin; i < frame.m_events_end; i = ( i + 1 ) % max_events )
	{
		event_struct const& event = m_events[i];

		cfg.write_mask(
			TEXT_CONFIG_WRITE_MASK( "{ \"pid\":1, \"tid\":1, \"ts\":%d, \"dur\":%d, \"ph\":\"X\", \"name\":\"%s\" },\n" ),
			(u32)( event.m_time_start * 1000.0f ), (u32)( ( event.m_time_finish - event.m_time_start ) * 1000.0f ), event.m_name );
	}
	
	cfg.write_mask( TEXT_CONFIG_WRITE_MASK( "{ \"pid\":1, \"tid\":1, \"ts\":0, \"dur\":%d, \"ph\":\"X\", \"name\":\"frame\" }\n" ), (u32)( frame.m_frame_time * 1000.0f ) );
	cfg.write_str( "]}" );

	sys::file f( in_file_name, sys::file::open_write );
	ASSERT( f.is_valid( ) );
	f.write( data, (pstr)cfg.get_pointer( ) - data );
	f.close( );

	virtual_allocator( ).deallocate( data );
}
*/


statistics::frame::frame( )
{
	g_statistics.begin_frame( );
	m_timer.start( );
}

statistics::frame::~frame( )
{
	float const time = (float)m_timer.elapsed( );
	g_statistics.end_frame( time );
}

statistics::debug_event::debug_event( LPCWSTR const in_name_wstr )
{
	g_statistics.begin_debug_event( in_name_wstr );
}

statistics::debug_event::~debug_event( )
{
	g_statistics.end_debug_event( );
}

statistics::profile_event::profile_event( pcstr const in_name )
	: m_event_index( g_statistics.begin_profile_event( in_name ) )
{ }

statistics::profile_event::~profile_event( )
{
	g_statistics.end_profile_event( m_event_index );
}


statistics g_statistics;

} // namespace render