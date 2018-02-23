#ifndef __render_mesh_collection_h_included_
#define __render_mesh_collection_h_included_

#include <types.h>
#include "buffer.h"

namespace render {

namespace mesh_collection {

enum mesh_type {
	mesh_type_box = 0,
	mesh_type_sphere,
	mesh_type_cylinder,
	mesh_type_cone,
	mesh_type_quad,
		
	mesh_type_count
}; // enum mesh_type
	
void create( );
void destroy( );
	
mesh* get( mesh_type in_mesh_type );

} // namespace mesh_collection

} // namespace render

#endif // #ifndef __render_mesh_collection_h_included_