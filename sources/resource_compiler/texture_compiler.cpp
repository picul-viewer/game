#include "texture_compiler.h"
#include <Windows.h>
#include <macros.h>
#include <lib/allocator.h>
#include <lib/fixed_string.h>
#include <lib/text_reader.h>
#include <system/path.h>
#include <system/file.h>
#include <system/file_iterator.h>

namespace resource_compiler {

void texture_compiler::create( pcstr const tex_conv_path )
{
	m_tex_conv_path = tex_conv_path;
}

void texture_compiler::compile( u64 const relevant_date, pcstr const input_file_name, pcstr const output_directory )
{
	pcstr const extension = sys::path::get_file_extension( input_file_name );

	if ( strings::equal( extension, ".texconfig" ) )
	{
		// This is config.
		return;
	}
	
	little_string file_name = sys::path::get_file_name( input_file_name );
	sys::path::remove_file_extension( file_name );
	sys::path output_path( output_directory );
	output_path += file_name + ".dds";
	
	sys::file_iterator fi;
	fi.create( output_path.c_str( ) );
	if ( fi.is_valid( ) )
		if ( fi.get_modification_time( ) > relevant_date )
		{
			LOG( "texture_compiler: texture update is not required: \"%s\"\n", output_path.c_str( ) );
			return;
		}

	str4096 command_line;

	command_line.append( "\"" );
	command_line.append( m_tex_conv_path );
	command_line.append( "\" -o \"" );
	command_line.append( output_directory );
	command_line.append( "\"" );

	str256 config_path = input_file_name;
	sys::path::remove_file_extension( config_path );
	config_path.append( ".texconfig" );

	sys::file config( config_path.c_str( ), sys::file::open_read );
	if ( !config.is_valid( ) )
	{
		LOG( "texture_compiler: texture config is missing: \"%s\"\n", config_path.c_str( ) );
		return;
	}

	uptr const size = config.size( );
	ASSERT( size < 8 * Kb );
	u8* const data = stack_allocate( size );
	config.read( data, size );

	config.close( );

	str256 str;

	text_reader r( data, size );
	r.read_str( str.data( ) );

	if ( str == "format" )
	{
		r.read_str( str.data( ) );
		command_line.append( " -f " );
		command_line.append( str );
		
		r.read_str( str.data( ) );
	}
	
	if ( str == "mipmaps" )
	{
		r.read_str( str.data( ) );
		command_line.append( " -m " );
		command_line.append( str );
		
		r.read_str( str.data( ) );
	}

	if ( str == "srgb" )
	{
		r.read_str( str.data( ) );
		if ( str == "input" )
			command_line.append( " -srgbi" );
		else if ( str == "output" )
			command_line.append( " -srgbo" );
		else if ( str == "both" )
			command_line.append( " -srgb" );
		else
			ASSERT( str == "none" );

		r.read_str( str.data( ) );
	}

	command_line.append( " -nologo -y \"" );
	command_line.append( input_file_name );
	command_line.append( "\"" );
	
	STARTUPINFO si{ };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = nullptr;
	si.hStdOutput = nullptr;
	si.hStdError = nullptr;

	PROCESS_INFORMATION pi;

	BOOL const result = CreateProcess( nullptr, command_line.data( ), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );
	ASSERT( result != FALSE );
	
	WaitForSingleObject( pi.hProcess, INFINITE );

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	fi.create( output_path.c_str( ) );
	if ( fi.is_valid( ) && ( fi.get_modification_time( ) >= relevant_date ) )
		LOG( "texture_compiler: texture compiled successfully: \"%s\"\n", output_path.c_str( ) );
	else
		LOG( "texture_compiler: texture compilation failed: \"%s\"\n", output_path.c_str( ) );
}

} // namespace resource_compiler