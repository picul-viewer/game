#include "resource_compiler.h"

#include <Shlwapi.h>

namespace resource_compiler {

inline u64 filetime_to_u64( FILETIME data )
{
	return *(u64*)&data;
}

category::category( weak_const_string input_path, weak_const_string output_path, resource_compiler::strategy* strategy, u32 root_length ) :
	input_path	( input_path ),
	output_path	( output_path ),
	strategy	( strategy ),
	root_length	( root_length )
{ }

manager::manager( uptr category_count ) :
	m_input_path	( nullptr ),
	m_output_path	( nullptr )
{
	m_categories.reserve( category_count );
}

void manager::add_category( strategy* strategy, weak_const_string input_path, weak_const_string output_path, u32 root_length )
{
	m_categories.push_back( { input_path, output_path, strategy, root_length } );
}

void manager::compile( weak_const_string input_path, weak_const_string output_path )
{
	m_input_path	= input_path;
	m_output_path	= output_path;

	for ( std::vector<category>::iterator i = m_categories.begin( ), e = m_categories.end( ); i != e; ++i )
		compile_category( *i );
}

void manager::compile_category( category& category )
{
	str512 search_path( m_input_path );
	PathAppend( search_path.data( ), category.input_path.c_str( ) );

	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile( search_path.c_str( ), &file_data );

	if ( file_handle != INVALID_HANDLE_VALUE )
	{
		category.strategy->on_start( );

		do
		{
			if ( category.root_length )
			{
				str512 relative_path = str512( file_data.cFileName + category.root_length );
				PathRemoveFileSpec( relative_path.data( ) );

				str512 output_path = str512( category.output_path );
				PathAppend( output_path.data( ), relative_path.c_str( ) );

				category.strategy->compile( filetime_to_u64( file_data.ftLastWriteTime ), file_data.cFileName, output_path );
			}
			else
				category.strategy->compile( filetime_to_u64( file_data.ftLastWriteTime ), file_data.cFileName, category.output_path );
		}
		while ( FindNextFile( file_handle, &file_data ) );

		FindClose( file_handle );
		
		category.strategy->on_finish( );
	}
}

} // namespace resource_compiler