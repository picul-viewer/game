#include "time.h"

#include <Windows.h>

namespace sys {

static struct timer_data
{
	double period;

	timer_data( )
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency( &freq );
		period = 1.0 / (double)freq.QuadPart;
	}

	double current_time( ) const
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter( &time );
		return (double)time.QuadPart * period;
	}
} td;

time::time( double const v ) :
	m_time( v )
{ }

time time::from_ns( u64 const v )
{
	return time( v * 1e-9 );
}

time time::from_us( u64 const v )
{
	return time( v * 1e-6 );
}

time time::from_ms( u64 const v )
{
	return time( v * 1e-3 );
}

time time::from_s( u64 const v )
{
	return time( (double)v );
}

time time::now( )
{
	return time( td.current_time( ) );
}

u64 time::ns( ) const
{
	return (u64)( m_time * 1e9 );
}

u64 time::us( ) const
{
	return (u64)( m_time * 1e6 );
}

u64 time::ms( ) const
{
	return (u64)( m_time * 1e3 );
}

u64 time::s( ) const
{
	return (u64)m_time;
}

time::operator double( ) const
{
	return m_time;
}

time& time::operator+=( time const t )
{
	*this = *this + t;
	return *this;
}

time& time::operator-=( time const t )
{
	*this = *this - t;
	return *this;
}


void sys::timer::start( )
{
	m_start_time = time::now( );
}

time sys::timer::elapsed( ) const
{
	return time::now( ) - m_start_time;
}


time sys::ticker::tick( )
{
	time const now = time::now( );
	time const result = now - m_start_time;
	m_start_time = now;
	return result;
}

} // namespace sys