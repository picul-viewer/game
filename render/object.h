#ifndef __render_object_h_included_
#define __render_object_h_included_

#include <types.h>
#include <math/matrix.h>
#include <lib/embedded_list.h>
#include <lib/binary_config.h>

#include "render_object.h"

namespace render {

class object
{
public:
	void create( binary_config& in_config );
	void destroy( );
	
	void update( math::float4x3 const& in_transform );

	template<typename Pred>
	inline void for_each( Pred const& functor )
	{
		m_objects.for_each( functor );
	}

protected:
	typedef embedded_typeless_list render_object_list;
	render_object_list	m_objects;
};

} // namespace render

#endif // #ifndef __render_object_h_included_