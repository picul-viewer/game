#include "resource_types.h"
#include <lib/strings.h>

namespace resource_system {

uptr file_cook_data::calculate_size( ) const
{
	return strings::length( file_path ) + 1;
}

uptr request_cook_data::calculate_size( ) const
{
	return strings::length( file_path ) + 1;
}

} // namespace resource_system