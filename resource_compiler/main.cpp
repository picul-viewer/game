#include <macros.h>
#include <utils/resources_path.h>
#include "resource_compiler.h"

#define RESOURCES_SOURCES_PATH "..\\..\\resources\\sources"

int main( int argc, char** argv )
{
	resource_compiler::resource_compiler rc;

	rc.create( );
	rc.compile( RESOURCES_SOURCES_PATH, RESOURCES_PATH );
	rc.destroy( );
}