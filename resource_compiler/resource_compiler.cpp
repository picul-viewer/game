#include "resource_compiler.h"

#include <Shlwapi.h>

namespace resource_compiler {

manager::manager( std::initializer_list<category> const& init ) :
	m_categories	( init ),
	m_input_path	( nullptr ),
	m_output_path	( nullptr )
{
#ifdef DEBUG
	for ( std::vector<category>::iterator i = m_categories.begin( ), e = m_categories.end( ); i != e; ++i )
		ASSERT( i->strategy );
#endif // #ifdef DEBUG
}

manager::~manager( )
{
	for ( std::vector<category>::iterator i = m_categories.begin( ), e = m_categories.end( ); i != e; ++i )
		delete i->strategy;
}

bool operator<=( FILETIME const& l, FILETIME const& r )
{
	if ( l.dwHighDateTime != r.dwHighDateTime )
		return l.dwHighDateTime < r.dwHighDateTime;
	return l.dwLowDateTime <= r.dwLowDateTime;
}

void manager::compile( weak_const_string input_path, weak_const_string output_path )
{
	for ( std::vector<category>::iterator i = m_categories.begin( ), e = m_categories.end( ); i != e; ++i )
		compile_category( *i );
}

void manager::compile_category( category& category )
{
	str256 const& search_path = str512( m_input_path ) + category.path;

	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile( search_path.c_str( ), &file_data );

	if ( file_handle != INVALID_HANDLE_VALUE )
	{
		do
		{
			str256 file_local_path = str256( file_data.cFileName + m_input_path.length( ) );
			PathRemoveFileSpec( file_local_path.data( ) );

			str256 file_name = str256( file_data.cFileName + m_input_path.length( ) );
			PathFindFileName( file_name.data( ) );

			str256 output_file_name;
			category.strategy->get_output_name( file_name, output_file_name.data( ) );

			str256 const& output_dir = str256( m_output_path ) + file_local_path;
			str256 const& output_file = output_dir + output_file_name;

			WIN32_FIND_DATA output_data;

			if ( FindFirstFile( output_file.c_str( ), &output_data ) != INVALID_HANDLE_VALUE )
			{
				if ( file_data.ftLastWriteTime <= output_data.ftLastWriteTime )
					// Compiled data is relevant
					continue;
			}

			category.strategy->compile( file_data.cFileName, output_dir );
		}
		while ( FindNextFile( file_handle, &file_data ) );

		FindClose( file_handle );
	}
}

} // namespace resource_compiler