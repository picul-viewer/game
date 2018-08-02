#include <macros.h>
#include "resource_compiler.h"

int main( int argc, char** argv )
{
	ASSERT( argc >= 3 );

	resource_compiler::resource_compiler rc;

	rc.create( );
	rc.compile( argv[1], argv[2] );
	rc.destroy( );
}