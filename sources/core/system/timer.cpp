#include "timer.h"

#include <Windows.h>

namespace sys {

static struct TimerData
{
	u64 period;
	float frequency;
	double frequency_precise;
	inline TimerData( )
	{
		QueryPerformanceFrequency( (LARGE_INTEGER*)&period );
		frequency = 1.0f / period;
		frequency_precise = 1.0 / period;
	}
} timer_data;

u64 tick( )
{
	u64 now;
	QueryPerformanceCounter( (LARGE_INTEGER*)&now );
	return now;
}

float tick_to_time( u64 const tick )
{
	return tick * timer_data.frequency;
}

float time( )
{
	return tick_to_time( tick( ) );
}

double tick_to_time_precise( u64 const tick )
{
	return tick * timer_data.frequency_precise;
}

double time_precise( )
{
	return tick_to_time_precise( tick( ) );
}


void timer::start( )
{
	m_start_time = tick( );
}

u64 timer::get_elapsed_ns( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * 1000000000u / timer_data.period;
}

u64 timer::get_elapsed_us( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * 1000000u / timer_data.period;
}

u64 timer::get_elapsed_ms( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * 1000u / timer_data.period;
}

u64 timer::get_elapsed_s( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) / timer_data.period;
}


void float_timer::start( )
{
	m_start_time = tick( );
}

float float_timer::get_elapsed_time( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * timer_data.frequency;
}


void double_timer::start( )
{
	m_start_time = tick( );
}

double double_timer::get_elapsed_time( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * timer_data.frequency_precise;
}


u64 ticker::tick_ns( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) * 1000000000u / timer_data.period;
	m_start_time = now;
	return result;
}

u64 ticker::tick_us( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) * 1000000u / timer_data.period;
	m_start_time = now;
	return result;
}

u64 ticker::tick_ms( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) * 1000u / timer_data.period;
	m_start_time = now;
	return result;
}

u64 ticker::tick_s( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) / timer_data.period;
	m_start_time = now;
	return result;
}


float float_ticker::tick( )
{
	u64 const now = sys::tick( );
	float const result = ( now - m_start_time ) * timer_data.frequency;
	m_start_time = now;
	return result;
}


double double_ticker::tick( )
{
	u64 const now = sys::tick( );
	double const result = ( now - m_start_time ) * timer_data.frequency_precise;
	m_start_time = now;
	return result;
}

} // namespace sys