#ifndef __render_texture_h_included_
#define __render_texture_h_included_

#include <types.h>

#include <lib/reader.h>
#include <lib/weak_string.h>
#include "resource_views.h"
#include "resource_handle.h"

namespace render {

template<typename Resource>
class resource_pool;

class texture : public shader_resource_view
{
public:
	void create( reader& in_reader );

	u32 add_ref( ) const;
	u32 release( ) const;
	
public:
	static texture* from_handle( texture_id const in_id );
	static texture_id to_handle( texture* const in_texture );

private:
	friend class resource_pool<texture>;

	void set_registry_pointer( pointer const in_pointer );
	pointer get_registry_pointer( ) const;

private:
	pointer m_registry_pointer;

};

DEFINE_HANDLE( texture );

} // namespace render

#endif // #ifndef __render_texture_h_included_