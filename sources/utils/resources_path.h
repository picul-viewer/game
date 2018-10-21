#ifndef __utils_resources_path_h_included_
#define __utils_resources_path_h_included_

#include <types.h>
#include <system/path.h>

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "..\\..\\resources\\build\\"
#endif // #ifndef RESOURCES_PATH

#define GET_RESOURCE_PATH( path ) RESOURCES_PATH path

namespace utils {

sys::path get_resource_path( pcstr relative_path );

} // namespace utils

#endif // #ifndef __utils_resources_path_h_included_