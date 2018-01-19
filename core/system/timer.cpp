#include "timer.h"

#include <Windows.h>

static struct TimerData
{
	u64 f;
	float p;
	inline TimerData( )
	{
		QueryPerformanceFrequency( (LARGE_INTEGER*)&f );
		p = 1.0f / f;
	}
} timer_data;

void timer::start( )
{
	QueryPerformanceCounter( (LARGE_INTEGER*)&start_time );
}

u64 timer::get_elapsed_ns( ) const
{
	u64 elapsed;
	QueryPerformanceCounter( (LARGE_INTEGER*)&elapsed );
	return ( elapsed - start_time ) * 1000000000u / timer_data.f;
}

u64 timer::get_elapsed_us( ) const
{
	u64 elapsed;
	QueryPerformanceCounter( (LARGE_INTEGER*)&elapsed );
	return ( elapsed - start_time ) * 1000000u / timer_data.f;
}

u64 timer::get_elapsed_ms( ) const
{
	u64 elapsed;
	QueryPerformanceCounter( (LARGE_INTEGER*)&elapsed );
	return ( elapsed - start_time ) * 1000u / timer_data.f;
}

u64 timer::get_elapsed_s( ) const
{
	u64 elapsed;
	QueryPerformanceCounter( (LARGE_INTEGER*)&elapsed );
	return ( elapsed - start_time ) / timer_data.f;
}


float time_tick( )
{
	u64 elapsed;
	QueryPerformanceCounter( (LARGE_INTEGER*)&elapsed );
	return elapsed * timer_data.p;
}


void float_timer::start( )
{
	QueryPerformanceCounter( (LARGE_INTEGER*)&start_time );
}

float float_timer::get_elapsed_time( ) const
{
	u64 elapsed;
	QueryPerformanceCounter( (LARGE_INTEGER*)&elapsed );
	return ( elapsed - start_time ) * timer_data.p;
}