#include "shader_strategy.h"

#include <lib/allocator.h>
#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include <game/text_config.h>

#include <Shlwapi.h>

namespace resource_compiler {

shader_strategy::shader_strategy( weak_const_string version, bool debug ) :
	m_version	( version ),
	m_debug		( debug )
{ }

void shader_strategy::compile( u64 relevant_date, weak_const_string input_path, weak_const_string output_directory )
{
	HANDLE file = CreateFile( input_path.c_str( ), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );

	DWORD file_size;
	GetFileSize( file, &file_size );
	
	pointer data = mem_allocator( ).allocate( (uptr)file_size );

	DWORD bytes_read = 0;
	ReadFile( file, data, file_size, &bytes_read, nullptr );
	CloseHandle( file );

	str512 input_directory = input_path;
	PathRemoveFileSpec( input_directory.data( ) );
	
	text_config config( data );
	str256 shader_name;

	enum { max_executed_handles = 16 };
	u32 handle_index = 0;
	HANDLE process_handles[max_executed_handles];

	while ( config.read_str( shader_name.data( ) ), shader_name.length( ) != 0 )
	{
		u64 configuration = config.read<u64>( );
		
		fixed_string<17> configuration_string;
		sprintf( configuration_string.data( ), "%016llx", configuration );
		
		str512 output_path = str512( output_directory );
		PathAppend( output_path.data( ), ( shader_name + weak_const_string( "_" ) + configuration_string ).c_str( ) );

		WIN32_FIND_DATA output_data;
		if ( FindFirstFile( output_path.c_str( ), &output_data ) != INVALID_HANDLE_VALUE )
			if ( filetime_to_u64( output_data.ftLastWriteTime ) > relevant_date )
				continue;

		str512 input_path = str512( input_directory );
		PathAppend( input_path.data( ), shader_name.c_str( ) );
		str512 errors_path = output_path + weak_const_string( "_errors" );

		fixed_string<4> shader_type = shader_name.copy( shader_name.length( ) - 2, 2 );

		str2048 command_line =
			str2048( "/T " ) + shader_type + weak_const_string( "_" ) + m_version + weak_const_string( " " ) +
			weak_const_string( "\"" ) + output_path + weak_const_string( "\" /D CONFIG = " ) + configuration_string +
			weak_const_string( m_debug ? " /nologo /E \"main\" /Zi /Od /Fe \"" : " /Zi /Od /Fe \"" ) +
			errors_path + weak_const_string( "\" /Fo \"" ) + output_path + weak_const_string( "\"" );

		process_handles[handle_index++] = execute( command_line );

		if ( handle_index == max_executed_handles )
		{
			WaitForMultipleObjects( max_executed_handles, process_handles, true, INFINITE );
			handle_index = 0;
		}
	}

	if ( handle_index != 0 )
		WaitForMultipleObjects( handle_index, process_handles, true, INFINITE );
}

pointer shader_strategy::execute( weak_string command_line )
{
	PROCESS_INFORMATION processInformation = { };
	STARTUPINFO startupInfo                = { };

	BOOL result = CreateProcess( NULL, command_line.data( ), 
								 NULL, NULL, FALSE, 
								 HIGH_PRIORITY_CLASS | CREATE_NO_WINDOW, 
								 NULL, NULL, &startupInfo, &processInformation);

	return processInformation.hProcess;
}

} // namespace resource_compiler