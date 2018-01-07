#include <stdio.h>

#include "resource_compiler.h"

using namespace resource_compiler;

int main( int argc, char** argv )
{
	ASSERT( argc >= 3 );

	manager manager( { } );

	manager.compile( argv[1], argv[2] );
}