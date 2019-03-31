#ifndef GUARD_CORE_TIMER_H_INCLUDED
#define GUARD_CORE_TIMER_H_INCLUDED

#include <types.h>

namespace sys {

u64 tick( );

float tick_to_time( u64 const tick );
float time( );

double tick_to_time_precise( u64 const tick );
double time_precise( );

class timer
{
public:
	void start( );
	
	u64 get_elapsed_ns( ) const;
	u64 get_elapsed_us( ) const;
	u64 get_elapsed_ms( ) const;
	u64 get_elapsed_s( ) const;

private:
	u64 m_start_time;
};

class float_timer
{
public:
	void start( );
	float get_elapsed_time( ) const;

private:
	u64 m_start_time;
};

class double_timer
{
public:
	void start( );
	double get_elapsed_time( ) const;

private:
	u64 m_start_time;
};

class ticker
{
public:
	u64 tick_ns( );
	u64 tick_us( );
	u64 tick_ms( );
	u64 tick_s( );

private:
	u64 m_start_time;
};

class float_ticker
{
public:
	float tick( );

private:
	u64 m_start_time;
};

class double_ticker
{
public:
	double tick( );

private:
	u64 m_start_time;
};

} // namespace sys

#endif // #ifndef GUARD_CORE_TIMER_H_INCLUDED