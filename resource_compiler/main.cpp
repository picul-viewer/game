#include <stdio.h>

#include "resource_compiler.h"

#include "shader_strategy.h"

using namespace resource_compiler;

int main( int argc, char** argv )
{
	ASSERT( argc >= 3 );

	manager manager( 2 );

	bool shaders_debug = 
#ifdef DEBUG
		true
#else
		false
#endif // #ifdef DEBUG
		;
		
	shader_strategy s0( "4_0", shaders_debug );
	shader_strategy s1( "4_0", shaders_debug );

	manager.add_category( &s0, "shaders/compile.cfg", "shaders/4_0/", 0 );
	manager.add_category( &s1, "shaders/compile.cfg", "shaders/5_0/", 0 );

	manager.compile( argv[1], argv[2] );
}