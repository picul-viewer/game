#ifndef GUARD_RENDER_DX_HEAP_H_INCLUDED
#define GUARD_RENDER_DX_HEAP_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

class dx_heap
{
public:
	void create(
		u64 const in_size,
		D3D12_HEAP_TYPE const in_type,
		bool const in_allow_textures = true,
		bool const in_allow_targets = true,
		bool const in_allow_buffers = true
	);
	void destroy( );

	inline void set( ID3D12Heap* in_heap ) { m_heap = in_heap; }

	inline operator ID3D12Heap*( ) const { return m_heap; }
	inline ID3D12Heap* operator->( ) const { return m_heap; }

private:
	ID3D12Heap* m_heap;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_HEAP_H_INCLUDED