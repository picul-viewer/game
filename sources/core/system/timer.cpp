#include "timer.h"

#include <Windows.h>

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

u64 sys::tick( )
{
	u64 now;
	QueryPerformanceCounter( (LARGE_INTEGER*)&now );
	return now;
}

float sys::tick_to_time( u64 const tick )
{
	return tick * timer_data.frequency;
}

float sys::time( )
{
	return tick_to_time( tick( ) );
}

double sys::tick_to_time_precise( u64 const tick )
{
	return tick * timer_data.frequency_precise;
}

double sys::time_precise( )
{
	return tick_to_time_precise( tick( ) );
}


void sys::timer::start( )
{
	m_start_time = tick( );
}

u64 sys::timer::get_elapsed_ns( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * 1000000000u / timer_data.period;
}

u64 sys::timer::get_elapsed_us( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * 1000000u / timer_data.period;
}

u64 sys::timer::get_elapsed_ms( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * 1000u / timer_data.period;
}

u64 sys::timer::get_elapsed_s( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) / timer_data.period;
}


void sys::float_timer::start( )
{
	m_start_time = tick( );
}

float sys::float_timer::get_elapsed_time( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * timer_data.frequency;
}


void sys::double_timer::start( )
{
	m_start_time = tick( );
}

double sys::double_timer::get_elapsed_time( ) const
{
	u64 const now = tick( );
	return ( now - m_start_time ) * timer_data.frequency_precise;
}


u64 sys::ticker::tick_ns( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) * 1000000000u / timer_data.period;
	m_start_time = now;
	return result;
}

u64 sys::ticker::tick_us( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) * 1000000u / timer_data.period;
	m_start_time = now;
	return result;
}

u64 sys::ticker::tick_ms( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) * 1000u / timer_data.period;
	m_start_time = now;
	return result;
}

u64 sys::ticker::tick_s( )
{
	u64 const now = tick( );
	u64 const result = ( now - m_start_time ) / timer_data.period;
	m_start_time = now;
	return result;
}


float sys::float_ticker::tick( )
{
	u64 const now = sys::tick( );
	float const result = ( now - m_start_time ) * timer_data.frequency;
	m_start_time = now;
	return result;
}


double sys::double_ticker::tick( )
{
	u64 const now = sys::tick( );
	double const result = ( now - m_start_time ) * timer_data.frequency_precise;
	m_start_time = now;
	return result;
}