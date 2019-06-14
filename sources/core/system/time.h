#ifndef GUARD_CORE_TIME_H_INCLUDED
#define GUARD_CORE_TIME_H_INCLUDED

#include <types.h>

namespace sys {

class time
{
public:
	time( ) = default;
	time( double const v );

	static time from_ns( u64 const v );
	static time from_us( u64 const v );
	static time from_ms( u64 const v );
	static time from_s( u64 const v );

	static time now( );

	u64 ns( ) const;
	u64 us( ) const;
	u64 ms( ) const;
	u64 s( ) const;

	operator double( ) const;

	time& operator+=( time const t );
	time& operator-=( time const t );

private:
	double m_time;

};

class timer
{
public:
	void start( );
	
	time elapsed( ) const;

private:
	time m_start_time;
};

class ticker
{
public:
	time tick( );

private:
	time m_start_time;
};

} // namespace sys

#endif // #ifndef GUARD_CORE_TIME_H_INCLUDED