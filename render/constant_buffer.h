#ifndef __render_constant_buffer_h_included_
#define __render_constant_buffer_h_included_

#include <types.h>
#include "dx_include.h"
#include "buffer.h"

namespace render {

class constant_buffer
{
public:
	void create( pvoid in_data, uptr in_size, bool in_is_dynamic = false );
	void destroy( );
	
	u32 add_ref( ) const;
	u32 release( ) const;
	
	inline void set( ID3D11Buffer* in_buffer );
	inline ID3D11Buffer* const& get( ) const;

	template<typename Data>
	void update( Data const& in_data ) const;
	void update( pcvoid in_data, u32 in_offset, u32 in_size ) const;

	void bind_vs( u32 in_slot ) const;
	void bind_ps( u32 in_slot ) const;
	void bind_gs( u32 in_slot ) const;
	void bind_hs( u32 in_slot ) const;
	void bind_ds( u32 in_slot ) const;
	void bind_cs( u32 in_slot ) const;

protected:
	buffer m_buffer;

};

} // namespace render

#include "constant_buffer_inline.h"

#endif // #ifndef __render_constant_buffer_h_included_