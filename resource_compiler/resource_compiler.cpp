#include "resource_compiler.h"

#include <Shlwapi.h>

namespace resource_compiler {

inline u64 filetime_to_u64( FILETIME data )
{
	return *(u64*)&data;
}

category::category( weak_const_string input_path, weak_const_string output_path, weak_const_string filename_ending, resource_compiler::strategy* strategy ) :
	input_path		( input_path ),
	output_path		( output_path ),
	filename_ending	( filename_ending ),
	strategy		( strategy )
{ }

manager::manager( uptr category_count ) :
	m_input_path	( nullptr ),
	m_output_path	( nullptr )
{
	m_categories.reserve( category_count );
}

void manager::add_category( weak_const_string input_path, weak_const_string output_path, weak_const_string filename_ending, resource_compiler::strategy* strategy )
{
	m_categories.push_back( { input_path, output_path, filename_ending, strategy } );
}

static inline void create_directory_tree( weak_const_string path )
{
	if ( path.length( ) == 0 )
		return;

	if ( !PathFileExists( path.c_str( ) ) )
	{
		str512 local_path( path );
		PathRemoveFileSpec( local_path.data( ) );

		create_directory_tree( local_path );
		CreateDirectory( path.c_str( ), nullptr );
	}
}

void manager::compile( weak_const_string input_path, weak_const_string output_path )
{
	m_input_path	= input_path;
	m_output_path	= output_path;

	create_directory_tree( output_path );

	for ( std::vector<category>::iterator i = m_categories.begin( ), e = m_categories.end( ); i != e; ++i )
		compile_category( *i );
}

void manager::scan( uptr input_path_size, str512& input_path, uptr output_path_size, str512& output_path, uptr filename_ending_size, weak_const_string filename_ending, resource_compiler::strategy* strategy )
{
	if ( !PathFileExists( output_path.c_str( ) ) )
		CreateDirectory( output_path.c_str( ), nullptr );
	
	// creating mask
	input_path += weak_const_string( "/*" );

	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile( input_path.c_str( ), &file_data );
	
	input_path.resize( input_path_size );

	if ( file_handle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( ( weak_const_string( file_data.cFileName ) == weak_const_string( "." ) ) ||
				 ( weak_const_string( file_data.cFileName ) == weak_const_string( ".." ) ) )
				continue;

			uptr length = weak_const_string( file_data.cFileName ).length( );

			if ( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				input_path += weak_const_string( "/" );
				input_path += weak_const_string( file_data.cFileName );

				output_path += weak_const_string( "/" );
				output_path += weak_const_string( file_data.cFileName );
								
				scan( input_path_size + length + 1, input_path, output_path_size + length + 1, output_path, filename_ending_size, filename_ending, strategy );

				input_path.resize( input_path_size );
				output_path.resize( output_path_size );
			}
			else if ( weak_const_string( file_data.cFileName + length - filename_ending_size ) == filename_ending )
			{
				input_path += weak_const_string( "/" );
				input_path += weak_const_string( file_data.cFileName );

				strategy->compile( filetime_to_u64( file_data.ftLastWriteTime ), input_path, output_path );
				
				input_path.resize( input_path_size );
			}
		}
		while ( FindNextFile( file_handle, &file_data ) );

		FindClose( file_handle );
	}
}

void manager::compile_category( category& category )
{
	category.strategy->on_start( );

	str512 input_path( m_input_path );
	PathAppend( input_path.data( ), category.input_path.c_str( ) );
	
	str512 output_path( m_output_path );
	PathAppend( output_path.data( ), category.output_path.c_str( ) );
	
	output_path.replace( '/', '\\' );

	create_directory_tree( output_path );

	scan( input_path.length( ), input_path, output_path.length( ), output_path, category.filename_ending.length( ), category.filename_ending, category.strategy );

	category.strategy->on_finish( );
}

} // namespace resource_compiler