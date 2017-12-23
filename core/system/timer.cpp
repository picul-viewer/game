#include "timer.h"

static struct Frequency
{
	LARGE_INTEGER f;
	inline Frequency( )
	{
		QueryPerformanceFrequency( &f );
	}
} freq;

void timer::start( )
{
	QueryPerformanceCounter( &start_time );
}

u64 timer::get_elapsed_ns( ) const
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter( &elapsed );
	return ( elapsed.QuadPart - start_time.QuadPart ) * 1000000000u / freq.f.QuadPart;
}

u64 timer::get_elapsed_us( ) const
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter( &elapsed );
	return ( elapsed.QuadPart - start_time.QuadPart ) * 1000000u / freq.f.QuadPart;
}

u64 timer::get_elapsed_ms( ) const
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter( &elapsed );
	return ( elapsed.QuadPart - start_time.QuadPart ) * 1000u / freq.f.QuadPart;
}

u64 timer::get_elapsed_s( ) const
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter( &elapsed );
	return ( elapsed.QuadPart - start_time.QuadPart ) / freq.f.QuadPart;
}


static struct Period
{
	float p;
	inline Period( )
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency( &freq );
		p = 1.0f / freq.QuadPart;
	}
} per;

float time_tick( )
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter( &elapsed );
	return elapsed.QuadPart * per.p;
}


void float_timer::start( )
{
	QueryPerformanceCounter( &start_time );
}

float float_timer::get_elapsed_time( ) const
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter( &elapsed );
	return ( elapsed.QuadPart - start_time.QuadPart ) * per.p;
}