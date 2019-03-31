#ifndef GUARD_RENDER_OBJECT_H_INCLUDED
#define GUARD_RENDER_OBJECT_H_INCLUDED

#include <types.h>
#include <math/matrix.h>
#include <lib/list.h>
#include <lib/reader.h>

#include "render_object.h"

namespace render {

class object
{
public:
	void create( lib::reader& in_reader );
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

#endif // #ifndef GUARD_RENDER_OBJECT_H_INCLUDED