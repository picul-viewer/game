#ifndef __render_object_h_included_
#define __render_object_h_included_

#include <types.h>
#include <math/matrix.h>
#include <lib/list.h>
#include <lib/reader.h>

#include "render_object.h"

namespace render {

class object
{
public:
	void create( reader& in_reader );
	void destroy( );
	
	void update( math::float4x3 const& in_transform );

	template<typename Pred>
	inline void for_each( Pred const& functor )
	{
		lib::for_each( m_objects.begin( ), m_objects.end( ), functor );
	}

protected:
	typedef lib::intrusive_list<render_object, &render_object::m_next> render_object_list;
	render_object_list	m_objects;
};

} // namespace render

#endif // #ifndef __render_object_h_included_