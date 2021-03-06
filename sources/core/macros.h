#ifndef GUARD_CORE_MACROS_H_INCLUDED
#define GUARD_CORE_MACROS_H_INCLUDED

#include <types.h>
#include <stdio.h>

#ifdef DEBUG
#define USE_ASSERTIONS
#endif // #ifdef DEBUG

#define NO_OP (void*)0

namespace macros {

void debug_string( char const* message );

template<typename ... Args>
inline void logger( char const* message, Args ... args )
{
	char text[8192] = { };
	sprintf( text, message, args ... );
	debug_string( text );
}

}

#define LOG( message, ... ) macros::logger( message, ##__VA_ARGS__ )

#ifdef USE_ASSERTIONS
#	define DEBUG_BREAK( ) __debugbreak( )
#endif // #ifdef USE_ASSERTIONS

#ifdef USE_ASSERTIONS
namespace macros {

inline void assertion_failed( char const* expr, char const* file, char const* func, int line )
{
	LOG( "ASSERTION FAILED:\nfile: %s\nfunction: %s\nline: %d\nexpression: %s\n", file, func, line, expr );
}

template<typename ... Args>
inline void assertion_failed( char const* expr, char const* file, char const* func, int line, char const* message, Args ... args )
{
	char text[8192] = { };
	sprintf( text, message, args ... );
	LOG( "ASSERTION FAILED:\nfile: %s\nfunction: %s\nline: %d\nexpression: %s\ninformation: %s\n", file, func, line, expr, text );
}

template<typename T>
inline void log_value( T value )
{
	LOG( "%u", (u32)value );
}

template<>
inline void log_value( s32 const value )
{
	LOG( "%d", value );
}

template<>
inline void log_value( u32 const value )
{
	LOG( "%u", value );
}

template<>
inline void log_value( s64 const value )
{
	LOG( "%lld", value );
}

template<>
inline void log_value( u64 const value )
{
	LOG( "%llu", value );
}

template<>
inline void log_value( float const value )
{
	LOG( "%f", value );
}

template<>
inline void log_value( double const value )
{
	LOG( "%lf", value );
}

template<typename T>
inline void log_value( T* const value )
{
	LOG( "0x%016llx", (uptr)value );
}

template<>
inline void log_value( pointer const value )
{
	LOG( "0x%016llx", (uptr)value );
}

template<>
inline void log_value( pcstr const value )
{
	LOG( value );
}

}

#	define ASSERT( expr, ... ) if ( !( expr ) ) { macros::assertion_failed( #expr, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); DEBUG_BREAK( ); }
#	define ASSERT_CMP( l, s, r, ... ) if ( !( ( l ) s ( r ) ) ) { macros::assertion_failed( #l " " #s " " #r, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); LOG( "l = " ); macros::log_value( l ); LOG( ", r = " ); macros::log_value( r ); LOG( "\n" ); DEBUG_BREAK( ); }
#else // #ifdef USE_ASSERTIONS
#	define ASSERT( expr, ... ) NO_OP
#	define ASSERT_CMP( l, s, r, ... ) NO_OP
#endif // #ifdef USE_ASSERTIONS

#ifdef USE_ASSERTIONS
namespace macros {

inline void fatal_error( char const* file, char const* func, int line )
{
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\n\n", file, func, line );
}

template<typename ... Args>
inline void fatal_error( char const* file, char const* func, int line, char const* message, Args ... args )
{
	char text[8192] = { };
	sprintf( text, message, args ... );
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\nmessage: %s\n", file, func, line, text );
}

}

#	define FATAL_ERROR( ... ) macros::fatal_error( __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ), DEBUG_BREAK( )
#else // #ifdef USE_ASSERTIONS
#	define FATAL_ERROR( ... ) NO_OP
#endif // #ifdef USE_ASSERTIONS

#ifdef USE_ASSERTIONS
#	define UNREACHABLE_CODE { FATAL_ERROR( "unreachable code" ); } 
#else
namespace macros {
inline __declspec(noreturn) void noreturn_func( ) { }
}

#	define UNREACHABLE_CODE { macros::noreturn_func( ); }
#endif // #ifdef USE_ASSERTIONS

namespace macros {

void pause( );
void sleep( u32 const milliseconds );

}

#define PAUSE( ) macros::pause( )

#define SPIN_LOCK( expression ) while ( expression ) macros::pause( )

#endif // #ifndef GUARD_CORE_MACROS_H_INCLUDED
