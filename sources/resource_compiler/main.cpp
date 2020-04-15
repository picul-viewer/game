#include <macros.h>
#include <resources/resources_path.h>
#include "resource_compiler.h"

int main( int argc, char** argv )
{
	resource_compiler::resource_compiler rc;

	rc.create( );
	rc.compile( RESOURCES_SOURCES_PATH, RESOURCES_PATH );
	rc.destroy( );
}