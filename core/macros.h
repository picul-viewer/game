#ifndef __core_macros_h_included_
#define __core_macros_h_included_

#include <windows.h>
#include <cstdio>

#define NO_OP {}

#define LOG( message, ... ) { \
	char text[8192] = {}; \
	sprintf( text, message, ##__VA_ARGS__ ); \
	OutputDebugString( text ); \
}

#ifdef DEBUG
#	define ASSERT( expr ) if ( !( expr ) ) \
{ \
	LOG( "ASSERTION FAILED:\nfile: %s\nfunction: %s\nline: %d\nexpression: %s\n", __FILE__, __FUNCTION__, __LINE__, #expr ); \
	throw -1; \
}
#else // #ifdef DEBUG
#	define ASSERT( expr ) NO_OP
#endif // #ifdef DEBUG

#ifdef DEBUG
#	define FATAL_ERROR( message ) \
{ \
	LOG( "ERROR:\nfile: %s\nfunction: %s\nline: %d\nmessage: %s\n", __FILE__, __FUNCTION__, __LINE__, message ); \
	throw -1; \
}
#else // #ifdef DEBUG
#	define FATAL_ERROR( message ) NO_OP
#endif // #ifdef DEBUG

#define UNREACHABLE_CODE FATAL_ERROR( "unreachable code" )

#define mem_align(n) __declspec(align(n))

#define aligned(p, n) ((uptr)p % n == 0)

#endif // #ifndef __core_macros_h_included_
