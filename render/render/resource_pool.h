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

	weak_const_string		m_resource_path;
	alloc_string_heap<8192>	m_pool_string_heap;
	pool_data				m_pool_data;
};

} // namespace render

#include "resource_pool_inline.h"

#endif // #ifndef __render_resource_pool_h_included_