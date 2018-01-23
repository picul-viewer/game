#include <stdio.h>

#include "resource_compiler.h"

#include "shader_strategy.h"
#include "fbx_mesh_strategy.h"

using namespace resource_compiler;

int main( int argc, char** argv )
{
	ASSERT( argc >= 3 );

	manager manager( 3 );

	bool shaders_debug = 
#ifdef DEBUG
		true
#else
		false
#endif // #ifdef DEBUG
		;
		
	shader_strategy s0( "4_0", shaders_debug );
	shader_strategy s1( "4_0", shaders_debug );
	fbx_mesh_strategy s2;

	manager.add_category( "shaders/", "shaders/4_0/", "compile.cfg"	, &s0 );
	manager.add_category( "shaders/", "shaders/5_0/", "compile.cfg"	, &s1 );
	manager.add_category( "meshes/"	, "meshes"		, ".fbx"		, &s2 );

	manager.compile( argv[1], argv[2] );
}