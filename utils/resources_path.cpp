#include "resources_path.h"

namespace utils {

sys::path get_resource_path( pcstr relative_path )
{
	sys::path path( RESOURCES_PATH );
	path += relative_path;
	return path;
}

} // namespace utils