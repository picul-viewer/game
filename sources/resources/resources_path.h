#ifndef GUARD_RESOURCES_RESOURCES_PATH_H_INCLUDED
#define GUARD_RESOURCES_RESOURCES_PATH_H_INCLUDED

#include <types.h>
#include <system/path.h>

#ifndef RESOURCES_SOURCES_PATH
#define RESOURCES_SOURCES_PATH "..\\..\\resources\\sources\\"
#endif // #ifndef RESOURCES_SOURCES_PATH

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "..\\..\\resources\\build\\"
#endif // #ifndef RESOURCES_PATH

#define GET_RESOURCE_SOURCE_PATH( path ) RESOURCES_SOURCES_PATH path

#define GET_RESOURCE_PATH( path ) RESOURCES_PATH path

namespace resources {

sys::path get_resource_path( pcstr const in_relative_path );
sys::path get_source_path_from_target_path( sys::path const in_path );
sys::path get_target_path_from_source_path( sys::path const in_path );

} // namespace resources

using resources::get_resource_path;

#endif // #ifndef GUARD_RESOURCES_RESOURCES_PATH_H_INCLUDED