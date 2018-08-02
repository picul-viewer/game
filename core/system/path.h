#ifndef __core_path_h_included_
#define __core_path_h_included_

#include <types.h>
#include <lib/basic_string.h>
#include <lib/fixed_string.h>

namespace sys {

class path
{
public:
	path( ) = default;

	path( pcstr str );
	template<typename StringClass>
	path( i_const_string<StringClass> const& str );

	pcstr c_str( ) const;

	path operator+( pcstr str ) const;
	template<typename StringClass>
	path operator+( i_const_string<StringClass> const& str ) const;

	path& operator+=( pcstr str );
	template<typename StringClass>
	path& operator+=( i_const_string<StringClass> const& str );

	bool is_valid( ) const;
	pcstr get_file_name( ) const;
	void create_directory( ) const;
	void remove_file_extension( );

public:
	static bool is_valid( pcstr in_path );
	static pcstr get_file_name( pcstr in_path );
	static void create_directory( pcstr in_path );
	static void remove_file_extension( pstr in_path );

protected:
	str256 m_string;

};

} // namespace sys

#include "path_inline.h"

#endif // #ifndef __core_path_h_included_