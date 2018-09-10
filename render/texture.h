#ifndef __render_texture_h_included_
#define __render_texture_h_included_

#include <types.h>

#include <lib/weak_string.h>
#include <lib/binary_config.h>
#include "resource_views.h"

namespace render {

class texture : public shader_resource_view
{
public:
	void create( binary_config& in_config );

	u32 add_ref( ) const;
	u32 release( ) const;

	void set_registry_pointer( pointer in_pointer );
	pointer get_registry_pointer( ) const;

protected:
	pointer m_registry_pointer;

};

} // namespace render

#endif // #ifndef __render_texture_h_included_