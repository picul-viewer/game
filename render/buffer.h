#ifndef __render_buffer_h_included_
#define __render_buffer_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

namespace render {

class buffer
{
public:
	buffer( );
	
	struct cook
	{
		void set_vertex_buffer( u32		in_size,
								bool	in_is_dynamic	= false );
		
		void set_index_buffer( u32 in_size  );
		
		void set_constant_buffer( u32	in_size,
								  bool	in_is_dynamic	= false );
		
		void set_buffer( u32			in_size,
						 D3D11_USAGE	in_usage,
						 u32			in_bind_flags,
						 u32			in_cpu_access_flags,
						 u32			in_misc_flags,
						 u32			in_structure_buffer_size );
		
		D3D11_BUFFER_DESC buffer_desc;
	};
	
	void create( cook const& in_cook, pcvoid in_data );
	void create( cook const& in_cook );
	void set( ID3D11Buffer* in_buffer );
	
	void add_ref( ) const;
	void release( ) const;
	
	void destroy( );

	void update_default( pcvoid in_data ) const;
	void update_default( pcvoid in_data, u32 in_offset, u32 in_size ) const;
	void update_dynamic( pcvoid in_data, u32 in_offset, u32 in_size ) const;
	
	inline ID3D11Buffer* get_resource( ) const { return m_buffer; }
	
protected:
	ID3D11Buffer*	m_buffer;
};

class vertex_buffer : public buffer
{
public:
	void create( cook const& in_cook, pcvoid in_data, u32 in_stride );

	void bind( u32 in_slot, u32 in_offset ) const;

protected:
	u32 m_stride;
};

class index_buffer : public buffer
{
public:
	void create( cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format );
	
	void bind( u32 in_offset ) const;

protected:
	DXGI_FORMAT		m_format;
};

class mesh
{
public:
	enum { max_vertex_buffers_count = 4 };
	
	void create( pointer in_data, uptr in_size );
	
	void set_vertex_buffers_count( u32 in_vertex_buffers_count );
	void create_vertex_buffer( u32 in_index, buffer::cook const& in_cook, pcvoid in_data, u32 in_stride );
	void create_index_buffer( buffer::cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format );
	void set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY in_primitive_topology );
	void set_dimensions( u32 in_index_count, u32 in_instance_count );
	
	void add_ref( ) const;
	void release( ) const;
	
	void destroy( );

	inline buffer const& get_vertex_buffer( u32 in_index ) const;
	inline buffer const& get_index_buffer( ) const;
	
	void bind( ) const;
	
	void draw( ) const;
	void draw( u32 in_index_count ) const;
	void draw_instanced( ) const;
	void draw_instanced( u32 in_instance_count ) const;
	
protected:
	buffer						m_vertex_buffers[max_vertex_buffers_count];
	buffer						m_index_buffer;
	u32							m_vertex_strides[max_vertex_buffers_count];
	DXGI_FORMAT					m_index_format;
	u32							m_index_count;
	u32							m_instance_count;
	u32							m_buffers_count;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitive_topology;
};

} // namespace render

#endif // #ifndef __render_buffer_h_included_