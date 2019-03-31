#ifndef GUARD_RENDER_BUFFER_H_INCLUDED
#define GUARD_RENDER_BUFFER_H_INCLUDED

#include <types.h>
#include "dx_include.h"

namespace render {

class buffer
{
public:	
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
	
	buffer( );

	void create( cook const& in_cook, pcvoid in_data );
	void create( cook const& in_cook );
	void destroy( );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	inline void set( ID3D11Buffer* in_buffer ) { m_buffer = in_buffer; }
	inline ID3D11Buffer* const& get( ) const { return m_buffer; }

	void update_default( pcvoid in_data ) const;
	void update_default( pcvoid in_data, u32 in_size, u32 in_offset = 0 ) const;
	void update_dynamic( pcvoid in_data, u32 in_size, u32 in_offset = 0 ) const;
	
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

#endif // #ifndef GUARD_RENDER_BUFFER_H_INCLUDED