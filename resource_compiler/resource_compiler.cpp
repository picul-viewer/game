#include "resource_compiler.h"

#include <Shlwapi.h>

namespace resource_compiler {

inline u64 filetime_to_u64( FILETIME data )
{
	return *(u64*)&data;
}

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

void manager::compile( weak_const_string input_path, weak_const_string output_path )
{
	for ( std::vector<category>::iterator i = m_categories.begin( ), e = m_categories.end( ); i != e; ++i )
		compile_category( *i );
}

void manager::compile_category( category& category )
{
	str256 const& search_path = str512( m_input_path ) + category.input_path;

	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile( search_path.c_str( ), &file_data );

	if ( file_handle != INVALID_HANDLE_VALUE )
	{
		category.strategy->on_start( );

		do
			category.strategy->compile( filetime_to_u64( file_data.ftLastWriteTime ), file_data.cFileName, category.output_path );
		while ( FindNextFile( file_handle, &file_data ) );

		FindClose( file_handle );
		
		category.strategy->on_finish( );
	}
}

} // namespace resource_compiler