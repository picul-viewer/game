#ifndef GUARD_UTILS_RESOURCES_PATH_H_INCLUDED
#define GUARD_UTILS_RESOURCES_PATH_H_INCLUDED

#include <types.h>
#include <system/path.h>

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "..\\..\\resources\\build\\"
#endif // #ifndef RESOURCES_PATH

#define GET_RESOURCE_PATH( path ) RESOURCES_PATH path

namespace utils {

sys::path get_resource_path( pcstr relative_path );

} // namespace utils

#endif // #ifndef GUARD_UTILS_RESOURCES_PATH_H_INCLUDED