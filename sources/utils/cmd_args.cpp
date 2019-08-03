#include "cmd_args.h"
#include "parse.h"
#include <Windows.h>

namespace utils {

void skip_to_next_arg( pcstr& s )
{
	while ( ( *s != '\0' ) && !is_space( *s ) )
	{
		if ( *s == '"' )
		{
			skip_str( s );
		}
		else
			++s;
	}

	skip_spaces( s );
}

void parse_cmd_line( pcstr const key, void( *on_found )( pcstr const, pointer const ), pointer const arg )
{
	uptr const l = strings::length( key );
	pcstr s = GetCommandLineA( );

	{
	startloop:

		skip_to_next_arg( s );

		if ( *s++ != '-' )
			return;

		if ( *s == '.' )
			goto startloop;

		for ( uptr i = 0; i < l; ++i, ++s )
			if ( *s != key[i] )
				goto startloop;

		if ( *s != '=' )
			goto startloop;
	}

	on_found( ++s, arg );
}

} // namespace utils