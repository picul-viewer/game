#ifndef __render_resource_pool_h_included_
#define __render_resource_pool_h_included_

#include <core/types.h>
#include <core/std.h>

namespace render {

template<typename T>
class resource_pool
{
public:
	resource_pool( );

	void set_resource_path( pcstr in_path );

	T* create_resource( weak_const_string const& in_filename );
	void free_resource( T* in_resource );

protected:
	typedef map<weak_const_string, T> pool_data;
	typedef dynamic_allocation_multipool<8192, 128> string_pool;

	weak_const_string	m_resource_path;
	string_pool			m_string_pool;
	pool_data			m_pool_data;
};

} // namespace render

#include "resource_pool_inline.h"

#endif // #ifndef __render_resource_pool_h_included_