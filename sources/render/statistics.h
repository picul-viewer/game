#ifndef GUARD_RENDER_STATISTICS_H_INCLUDED
#define GUARD_RENDER_STATISTICS_H_INCLUDED

#include <types.h>
#include "dx_include.h"
#include <system/timer.h>

namespace render {

class statistics
{
public:
	statistics( );

	void create( );
	void destroy( );
	
	void process_frame( uptr const in_delay );

	void output_render_statistics( uptr const in_delay, pstr const out_string, uptr const in_max_chars ) const;
	//void save_render_statistics( uptr const in_delay, pcstr in_file_name ) const;

	struct frame
	{
	public:
		frame( );
		~frame( );
	private:
		sys::float_timer m_timer;
	};

	struct debug_event
	{
	public:
		debug_event( LPCWSTR const in_name_wstr );
		~debug_event( );
	};
	
	struct profile_event
	{
	public:
		profile_event( pcstr const in_name );
		~profile_event( );
	private:
		u32 const m_event_index;
	};

protected:
	void begin_frame( );
	void end_frame( float const in_elapsed_cpu_time );
	
	void begin_debug_event( LPCWSTR const in_name_wstr );
	void end_debug_event( );
	
	u32 begin_profile_event( pcstr const in_name );
	void end_profile_event( u32 const in_event_index );
	
	enum : u32 {
		max_frames = 8,
		max_events = 256,
	};

	struct frame_struct
	{
		ID3D11Query* m_query;
		ID3D11Query* m_t0;
		ID3D11Query* m_t1;
		u32 m_events_begin;
		u32 m_events_end;
		float m_precision;
		float m_render_gpu_frame_time;
		float m_render_cpu_frame_time;
		float m_frame_time;

		frame_struct( );
	};

	struct event_struct
	{
		pcstr m_name;
		ID3D11Query* m_t0;
		ID3D11Query* m_t1;
		float m_time_start;
		float m_time_finish;

		event_struct( );
	};

	frame_struct m_frames[max_frames];
	event_struct m_events[max_events];
	
	uptr m_frame_index;
	u32 m_event_index;

	sys::float_ticker m_ticker;

};


extern statistics g_statistics;

} // namespace render

#ifdef USE_RENDER_PROFILING
#	define RENDER_FRAME statistics::frame statistics_frame_struct
#else
#	define RENDER_FRAME
#endif // #ifdef USE_RENDER_PROFILING

#define RENDER_DEBUG_EVENT( name ) statistics::debug_event statistics_debug_event_##name( L#name )

#ifdef USE_RENDER_PROFILING
#	define RENDER_PROFILE_EVENT( name ) statistics::profile_event statistics_profile_event_##name( #name )
#else
#	define RENDER_PROFILE_EVENT( name )
#endif // #ifdef USE_RENDER_PROFILING

#endif // #ifndef GUARD_RENDER_STATISTICS_H_INCLUDED