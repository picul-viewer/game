#include "macros.h"
#include <Windows.h>

namespace macros {

void debug_string( char const* message )
{
	OutputDebugString( message );
}

} // namespace macros