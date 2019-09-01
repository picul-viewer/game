#ifndef GUARD_RENDER_MESH_H_INCLUDED
#define GUARD_RENDER_MESH_H_INCLUDED

#include <types.h>
#include "dx_include.h"
#include <lib/reader.h>
#include <resource_system/shared_resource.h>
#include "buffer.h"
#include "mesh_cook.h"
#include "handles.h"

namespace render {

class mesh : public shared_resource<mesh, mesh_handle, 4096>
{
public:
	friend class mesh_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_main
	};

	static void destroy_resource( mesh* const in_resource );

public:
	enum { max_vertex_buffers_count = 4 };

private:
	buffer const& get_vertex_buffer( u32 in_index ) const;
	buffer const& get_index_buffer( ) const;
	DXGI_FORMAT get_index_format( ) const;
	D3D11_PRIMITIVE_TOPOLOGY get_primitive_topology( ) const;
	u32 get_index_count( ) const;
	
public:
	void bind( ) const;
	
	void draw( ) const;
	void draw( u32 in_index_count ) const;
	void draw_instanced( u32 in_instance_count ) const;

private:
	friend class resources;
	
	// This is to be used by mesh collection in render resources.
	void create_vertex_buffer( u32 in_index, buffer::cook const& in_cook, pcvoid in_data, u32 in_stride );
	void create_index_buffer( buffer::cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format );
	void set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY in_primitive_topology );
	void set_dimensions( u32 in_index_count );

private:
	buffer						m_vertex_buffers[max_vertex_buffers_count];
	buffer						m_index_buffer;
	u32							m_vertex_strides[max_vertex_buffers_count];
	DXGI_FORMAT					m_index_format;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitive_topology;
	u32							m_index_count;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_MESH_H_INCLUDED