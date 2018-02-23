#ifndef __render_render_object_h_included_
#define __render_render_object_h_included_

#include <types.h>
#include "dx_include.h"

namespace render {

enum render_object_type
{
	render_object_type_mesh = 0,

	// ...

	render_object_type_count
};

class render_object
{
public:
	render_object( render_object_type in_type );

	virtual void dispatch( ) const = 0;

	virtual void update_transform( );

	inline render_object_type get_type( ) const { return m_type; }

public:
	render_object*				m_next;
	const render_object_type	m_type;
};

} // namespace render

#endif // #ifndef __render_render_object_h_included_