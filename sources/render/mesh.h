#ifndef __render_mesh_h_included_
#define __render_mesh_h_included_

#include <types.h>
#include "dx_include.h"

#include <lib/weak_string.h>
#include <lib/binary_config.h>
#include "buffer.h"

namespace render {

class mesh
{
public:
	enum { max_vertex_buffers_count = 4 };
	
	void create( binary_config& in_config );

	u32 add_ref( ) const;
	u32 release( ) const;
	
	void set_registry_pointer( pointer in_pointer );
	pointer get_registry_pointer( ) const;

	void destroy( );

	inline buffer const& get_vertex_buffer( u32 in_index ) const;
	inline buffer const& get_index_buffer( ) const;
	
	void bind( ) const;
	
	void draw( ) const;
	void draw( u32 in_index_count ) const;
	void draw_instanced( ) const;
	void draw_instanced( u32 in_instance_count ) const;
	
	// This is to be used by mesh collection in render resources.
	void create_vertex_buffer( u32 in_index, buffer::cook const& in_cook, pcvoid in_data, u32 in_stride );
	void create_index_buffer( buffer::cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format );
	void set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY in_primitive_topology );
	void set_dimensions( u32 in_index_count, u32 in_instance_count );

protected:
	friend struct mesh_creator;

	buffer						m_vertex_buffers[max_vertex_buffers_count];
	buffer						m_index_buffer;
	u32							m_vertex_strides[max_vertex_buffers_count];
	DXGI_FORMAT					m_index_format;
	u32							m_index_count;
	u32							m_instance_count;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitive_topology;

	pointer						m_registry_pointer;
};

} // namespace render

#endif // #ifndef __render_mesh_h_included_