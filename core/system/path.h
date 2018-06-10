#ifndef __core_path_h_included_
#define __core_path_h_included_

#include <types.h>
#include <lib/fixed_string.h>

namespace sys {

class path
{
public:
	path( ) = default;
	path( str256 const& str );

	str256 const& get_string( ) const;
	operator str256( );

	path operator+( path const& p );
	path& operator+=( path const& p );

	bool is_valid( ) const;

protected:
	str256 m_string;

};

} // namespace sys

#endif // #ifndef __core_path_h_included_