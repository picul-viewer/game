#include "macros.h"
#include <Windows.h>

namespace macros {

void debug_string( char const* message )
{
	OutputDebugString( message );
}

void pause( )
{
	_mm_pause( );
}

void sleep( u32 const milliseconds )
{
	Sleep( milliseconds );
}

} // namespace macros