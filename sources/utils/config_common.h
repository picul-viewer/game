#ifndef GUARD_UTILS_CONFIG_COMMON_H_INCLUDED
#define GUARD_UTILS_CONFIG_COMMON_H_INCLUDED

#include <types.h>

namespace utils {

enum config_node_type
{
	config_node_type_bool = 0,
	config_node_type_int,
	config_node_type_float,
	config_node_type_string,
	config_node_type_vector,
	config_node_type_list,
	config_node_type_object,

	config_node_type_count
};

} // namespace utils

#endif // #ifndef GUARD_UTILS_CONFIG_COMMON_H_INCLUDED