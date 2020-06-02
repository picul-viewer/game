#include "resources_path.h"

#ifndef RESOURCES_SOURCES_PATH
#define RESOURCES_SOURCES_PATH "..\\..\\resources\\sources\\"
#endif // #ifndef RESOURCES_SOURCES_PATH

namespace resources {

static const sys::path s_resources_path( RESOURCES_PATH );
static const uptr s_resources_path_length = strings::length( s_resources_path.c_str( ) );
static const sys::path s_resources_sources_path( RESOURCES_SOURCES_PATH );
static const uptr s_resources_sources_path_length = strings::length( s_resources_sources_path.c_str( ) );

sys::path get_resource_path( pcstr const in_relative_path )
{
	sys::path path = s_resources_path;
	path += in_relative_path;
	return path;
}

sys::path get_resource_source_path(pcstr const in_relative_path)
{
	sys::path path = s_resources_sources_path;
	path += in_relative_path;
	return path;
}

sys::path get_source_path_from_target_path( sys::path const in_path )
{
	ASSERT( strings::compare_n( in_path.c_str( ), s_resources_path.c_str( ), s_resources_path_length ) == 0 );

	sys::path path = s_resources_sources_path;
	path += in_path.c_str( ) + s_resources_path_length;
	return path;
}

sys::path get_target_path_from_source_path( sys::path const in_path )
{
	ASSERT( strings::compare_n( in_path.c_str( ), s_resources_sources_path.c_str( ), s_resources_sources_path_length ) == 0 );

	sys::path path = s_resources_path;
	path += in_path.c_str( ) + s_resources_sources_path_length;
	return path;
}

} // namespace resources