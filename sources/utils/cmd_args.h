#ifndef GUARD_UTILS_CMD_ARGS_H_INCLUDED
#define GUARD_UTILS_CMD_ARGS_H_INCLUDED

#include <types.h>
#include <lib/fixed_string.h>
#include "parse.h"

namespace utils {

void parse_cmd_line( pcstr const key, void( *on_found )( pcstr const, pointer const ), pointer const arg );

template<typename T>
class cmd_arg
{
public:
	cmd_arg( pcstr const key, T const& default_value );

	operator T( ) const;
	T value( ) const;

private:
	static void on_found( pcstr const str, pointer const this_ptr );

private:
	T m_data;

};

typedef cmd_arg<bool> cmd_arg_bool;
typedef cmd_arg<s32> cmd_arg_s32;
typedef cmd_arg<u32> cmd_arg_u32;
typedef cmd_arg<float> cmd_arg_float;
typedef cmd_arg<str256> cmd_arg_str;

} // namespace utils

#include "cmd_args_inline.h"

#endif // #ifndef GUARD_UTILS_CMD_ARGS_H_INCLUDED