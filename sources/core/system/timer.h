#ifndef __core_timer_h_included_
#define __core_timer_h_included_

#include <types.h>

namespace sys {

u64 tick( );

float tick_to_time( u64 tick );
float time( );

double tick_to_time_precise( u64 tick );
double time_precise( );

class timer
{
public:
	void start( );
	
	u64 get_elapsed_ns( ) const;
	u64 get_elapsed_us( ) const;
	u64 get_elapsed_ms( ) const;
	u64 get_elapsed_s( ) const;

protected:
	u64 start_time;
};

class float_timer
{
public:
	void start( );
	float get_elapsed_time( ) const;

protected:
	u64 start_time;
};

class double_timer
{
public:
	void start( );
	double get_elapsed_time( ) const;

protected:
	u64 start_time;
};

} // namespace sys

#endif // #ifndef __core_timer_h_included_