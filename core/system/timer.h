#ifndef __core_timer_h_included_
#define __core_timer_h_included_

#include <core/types.h>

class timer
{
public:
	void start( );
	
	u64 get_elapsed_ns( ) const;
	u64 get_elapsed_us( ) const;
	u64 get_elapsed_ms( ) const;
	u64 get_elapsed_s( ) const;

protected:
	LARGE_INTEGER start_time;
};

float time_tick( );

class float_timer
{
public:
	void start( );
	float get_elapsed_time( ) const;

protected:
	LARGE_INTEGER start_time;
};

#endif // #ifndef __core_timer_h_included_