#include "config_compiler.h"
#include <macros.h>
#include <lib/allocator.h>
#include <lib/fixed_string.h>
#include <lib/text_config.h>
#include <system/path.h>
#include <system/file.h>
#include <system/file_iterator.h>
#include "config_compiler_macros.h"

namespace resource_compiler {

void config_compiler::create( )
{ }

#define CONFIG_TYPES( config_type_instance )		\
	config_type_instance( font )					\


DECLARE( CONFIG_TYPES )


void config_compiler::compile( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	pcstr const extension = sys::path::get_file_extension( input_file_name );

	little_string file_name = sys::path::get_file_name( input_file_name );
	sys::path::remove_file_extension( file_name );
	little_string const config_type = sys::path::get_file_extension( file_name ) + 1;
	sys::path::remove_file_extension( file_name );
	sys::path output_path = str512( output_directory );
	output_path += file_name + ".cfg";
	
	sys::file_iterator fi;
	fi.create( output_path.c_str( ) );
	if ( fi.is_valid( ) )
		if ( fi.get_modification_time( ) > relevant_date )
		{
			LOG( "config_compiler: config update is not required: \"%s\"\n", output_path.c_str( ) );
			return;
		}

	sys::file f( input_file_name, sys::file::open_read );
	ASSERT( f.is_valid( ) );

	uptr const size = f.size( );
	pvoid const data = stack_allocate( size );

	f.read( data, size );
	f.close( );

	bool result = false;
	bool found_type = false;

	COMPILE( CONFIG_TYPES );

	if ( !found_type )
		LOG( "config_compiler: unexpected config type: \"%s\"\n", input_file_name );
	else if ( result )
		LOG( "config_compiler: config compiled successfully: \"%s\"\n", output_path.c_str( ) );
	else
		LOG( "config_compiler: config compilation failed: \"%s\"\n", output_path.c_str( ) );
}

} // namespace resource_compiler