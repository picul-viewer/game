#ifndef __core_macros_h_included_
#define __core_macros_h_included_

#include <windows.h>
#include <stdio.h>

#define NO_OP {}

namespace macros {

template<typename ... Args>
inline void logger( char const* message, Args ... args )
{
	char text[8192] = { };
	sprintf( text, message, args ... );
	OutputDebugString( text );
}

}

#define LOG( message, ... ) macros::logger( message, ##__VA_ARGS__ );

#ifdef DEBUG
#	define DEBUG_BREAK { __debugbreak( ); }
#endif // #ifdef DEBUG

#ifdef DEBUG
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

}

#	define ASSERT( expr, ... ) if ( !( expr ) ) { macros::assertion_failed( #expr, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); DEBUG_BREAK }
#else // #ifdef DEBUG
#	define ASSERT( expr, ... ) NO_OP
#endif // #ifdef DEBUG

#ifdef DEBUG
namespace macros {

inline void fatal_error( char const* file, char const* func, int line )
{
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\n\n", file, func, line );
}

template<typename ... Args>
inline void fatal_error( char const* file, char const* func, int line, char const* message, Args ... args )
{
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\nmessage: %s\n", file, func, line, message, args ... );
}

}

#	define FATAL_ERROR( ... ) { macros::fatal_error( __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); DEBUG_BREAK }
#else // #ifdef DEBUG
#	define FATAL_ERROR( ... ) NO_OP
#endif // #ifdef DEBUG

#define UNREACHABLE_CODE FATAL_ERROR( "unreachable code" )

#define mem_align(n) __declspec(align(n))

#define aligned(p, n) ((uptr)p % n == 0)

#endif // #ifndef __core_macros_h_included_
