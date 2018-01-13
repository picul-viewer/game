#include <stdio.h>

#include "resource_compiler.h"

#include "shader_strategy.h"

using namespace resource_compiler;

int main( int argc, char** argv )
{
	ASSERT( argc >= 3 );

	manager manager( 1 );

	{
		bool debug = 
#ifdef DEBUG
			true
#else
			false
#endif // #ifdef DEBUG
			;
		
		manager.add_category( new shader_strategy( "4_0", debug ), "shaders/compile.cfg", "shaders/4_0/" );
		manager.add_category( new shader_strategy( "5_0", debug ), "shaders/compile.cfg", "shaders/5_0/" );
	}

	manager.compile( argv[1], argv[2] );
}