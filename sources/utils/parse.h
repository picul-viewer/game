#ifndef GUARD_UTILS_PARSE_H_INCLUDED
#define GUARD_UTILS_PARSE_H_INCLUDED

#include <types.h>
#include <lib/fixed_string.h>

namespace utils {

bool is_space( char const c );
bool is_digit( char const c );
bool is_letter( char const c );
bool is_identifier_symbol( char const c );

void skip_spaces( pcstr& s );
void skip_str( pcstr& s );

bool parse_value( pcstr& s, bool& value );
bool parse_value( pcstr& s, s32& value );
bool parse_value( pcstr& s, u32& value );
bool parse_value( pcstr& s, s64& value );
bool parse_value( pcstr& s, u64& value );
bool parse_value( pcstr& s, float& value );
bool parse_value( pcstr& s, double& value );
bool parse_value( pcstr& s, str256& value );

} // namespace utils

#endif // #ifndef GUARD_UTILS_PARSE_H_INCLUDED