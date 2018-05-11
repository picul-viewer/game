#ifndef __render_buffer_h_included_
#define __render_buffer_h_included_

#include <types.h>
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
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	void destroy( );

	void update_default( pcvoid in_data ) const;
	void update_default( pcvoid in_data, u32 in_offset, u32 in_size ) const;
	void update_dynamic( pcvoid in_data, u32 in_offset, u32 in_size ) const;
	
	inline ID3D11Buffer* get( ) const { return m_buffer; }
	
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

} // namespace render

#endif // #ifndef __render_buffer_h_included_