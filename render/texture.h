#ifndef __render_texture_h_included_
#define __render_texture_h_included_

#include <core/std.h>
#include "dx_include.h"
#include "resource_views.h"

namespace render {

class texture : public shader_resource_view
{
public:
	texture( );

	u32 add_ref( ) const;
	u32 release( ) const;

protected:
	friend class texture_creator;
};

class texture_creator
{
public:
	static void create( texture* out_resource, weak_const_string in_filename );

protected:
	static void on_read_from_file( pointer in_data, uptr in_size, texture* out_texture );
	static void on_read_from_file_command( pointer in_data, uptr in_size, texture* out_texture );
};

} // namespace render

#endif // #ifndef __render_texture_h_included_