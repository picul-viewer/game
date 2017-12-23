#ifndef __core_macros_h_included_
#define __core_macros_h_included_

#include <windows.h>
#include <cstdio>

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

inline void assertion_failed( char const* expr )
{
	LOG( "ASSERTION FAILED:\nfile: %s\nfunction: %s\nline: %d\nexpression: %s\n", __FILE__, __FUNCTION__, __LINE__, expr );
}

template<typename ... Args>
inline void assertion_failed( char const* expr, char const* message, Args ... args )
{
	char text[8192] = { };
	sprintf( text, message, args ... );
	LOG( "ASSERTION FAILED:\nfile: %s\nfunction: %s\nline: %d\nexpression: %s\ninformation: %s\n", __FILE__, __FUNCTION__, __LINE__, expr, text );
}

}

#	define ASSERT( expr, ... ) if ( !( expr ) ) { macros::assertion_failed( #expr, ##__VA_ARGS__ ); DEBUG_BREAK }
#else // #ifdef DEBUG
#	define ASSERT( expr, ... ) NO_OP
#endif // #ifdef DEBUG

#ifdef DEBUG
namespace macros {

inline void fatal_error( )
{
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\n\n", __FILE__, __FUNCTION__, __LINE__ );
}

template<typename ... Args>
inline void fatal_error( char const* message, Args ... args )
{
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\nmessage: %s\n", __FILE__, __FUNCTION__, __LINE__, message, args ... );
}

}

#	define FATAL_ERROR( ... ) { macros::fatal_error( ##__VA_ARGS__ ); DEBUG_BREAK }
#else // #ifdef DEBUG
#	define FATAL_ERROR( message ) NO_OP
#endif // #ifdef DEBUG

#define UNREACHABLE_CODE FATAL_ERROR( "unreachable code" )

#define mem_align(n) __declspec(align(n))

#define aligned(p, n) ((uptr)p % n == 0)

#endif // #ifndef __core_macros_h_included_
