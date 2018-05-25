#ifndef __render_profiler_h_included_
#define __render_profiler_h_included_

#ifdef RENDER_ENABLE_GPU_PROFILING

#include <types.h>
#include "dx_include.h"

namespace render {

class profiler
{
public:
	profiler( );

	void create( );
	void destroy( );

	void output_statistics( uptr in_delay, uptr in_captions_width, uptr in_offset_size_in_spaces, pstr out_string, uptr in_max_output_size );

	struct profiler_frame
	{
	public:
		profiler_frame( );
		~profiler_frame( );
	};

	struct profiler_event
	{
	public:
		profiler_event( pcstr in_name );
		~profiler_event( );
	};

protected:
	void begin_frame( );
	void end_frame( );
	
	void begin_event( pcstr in_name );
	void end_event( );

	enum {
		max_frames = 4,
		max_events = 256,
	};

	struct frame_struct
	{
		ID3D11Query* m_query;
		uptr m_events_begin;
		uptr m_events_end;

		frame_struct( );
	};

	struct event_struct
	{
		pcstr m_name;
		uptr m_next_index;
		ID3D11Query* m_t0;
		ID3D11Query* m_t1;

		event_struct( );
	};

	frame_struct m_frames[max_frames];
	event_struct m_events[max_events];
	
	uptr m_frame_index;
	uptr m_event_index;

};


extern profiler g_profiler;

} // namespace render

#define PROFILER_CREATE { g_profiler.create( ); }
#define PROFILER_DESTROY { g_profiler.destroy( ); }
#define PROFILE_FRAME profiler::profiler_frame __unused_profiler_frame_struct;
#define PROFILE_EVENT( name ) profiler::profiler_event __unused_profiler_event_struct( name );

#else

#define PROFILER_CREATE
#define PROFILER_DESTROY
#define PROFILE_FRAME
#define PROFILE_EVENT( name )

#endif // #ifdef RENDER_ENABLE_GPU_PROFILING

#endif // #ifndef __render_profiler_h_included_