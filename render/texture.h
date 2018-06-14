#ifndef __render_texture_h_included_
#define __render_texture_h_included_

#include <types.h>
#include "dx_include.h"

#include <lib/weak_string.h>
#include "resource_views.h"

namespace render {

class texture : public shader_resource_view
{
public:
	texture( );

	u32 add_ref( ) const;
	u32 release( ) const;

public:
	static void load( texture* out_resource, weak_const_string in_filename );

protected:
	friend struct texture_creator;
};

} // namespace render

#endif // #ifndef __render_texture_h_included_