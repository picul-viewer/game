#ifndef GUARD_RENDER_STATISTICS_H_INCLUDED
#define GUARD_RENDER_STATISTICS_H_INCLUDED

#include <types.h>
#include <lib/linear_allocator.h>
#include <lib/linear_set.h>
#include <lib/text_writer.h>
#include <system/time.h>
#include "dx.h"
#include "dx_command_list.h"
#include "dx_resource.h"
#include "dx_query_heap.h"

namespace render {

class statistics
{
public:
	void create( );
	void destroy( );
	
	void resolve( dx_command_list const& in_list, u32 const in_frame_index );

	void process( pstr const out_string, uptr const in_max_chars );

	/*struct debug_event
	{
	public:
		debug_event( LPCWSTR const in_name_wstr );
		~debug_event( );
	};*/
	
	struct profile_event
	{
	public:
		profile_event( dx_command_list const& in_list, u32 const in_frame_index, pcstr const in_name );
		~profile_event( );

	private:
		dx_command_list const& m_list;
		u32 const m_frame_index;

	};

private:
	enum : u32 {
		max_events = 8
	};

	struct event_data
	{
		pcstr m_name;
		event_data* m_child;
		event_data* m_next;
		u32 m_query_index;
		float m_current_value;
	};

	typedef lib::linear_set<u32, max_events * 2> event_set_type;

private:
	void process_event_data( event_data* const in_data, lib::text_writer& in_writer, u64* const in_query_result, u32 const in_depth );
	
	//void begin_debug_event( LPCWSTR const in_name_wstr );
	//void end_debug_event( );
	
	void begin_profile_event( dx_command_list const& in_list, u32 const in_frame_index, pcstr const in_name );
	void end_profile_event( dx_command_list const& in_list, u32 const in_event_index );

private:
	event_data m_events[max_events];
	byte m_events_memory[event_set_type::memory_size];
	event_set_type m_event_set;

	event_data* m_stack[max_events];

	dx_query_heap m_heap;
	dx_resource m_result_buffer;

	event_data* m_root;

	sys::ticker m_frame_ticker;
	float m_current_cpu_frame_time;

	double m_inv_frequency;

	u32 m_query_count;
	u32 m_stack_index;

};

#ifdef USE_RENDER_PROFILING
extern statistics g_statistics;
#endif // #ifdef USE_RENDER_PROFILING

} // namespace render

//#define RENDER_DEBUG_EVENT( name ) statistics::debug_event statistics_debug_event_##name( L#name )

#ifdef USE_RENDER_PROFILING
#	define PROFILE_EVENT( list, frame_index, name ) statistics::profile_event statistics_profile_event_##name( list, frame_index, #name )
#else
#	define PROFILE_EVENT( list, frame_index, name )
#endif // #ifdef USE_RENDER_PROFILING

#endif // #ifndef GUARD_RENDER_STATISTICS_H_INCLUDED