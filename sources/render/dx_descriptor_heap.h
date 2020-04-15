#ifndef GUARD_RENDER_DX_DESCRIPTOR_HEAP_H_INCLUDED
#define GUARD_RENDER_DX_DESCRIPTOR_HEAP_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

class dx_descriptor_heap
{
public:
	void create( D3D12_DESCRIPTOR_HEAP_TYPE const in_type, u32 const in_descriptor_count, bool const in_shader_visible );
	void destroy( );

	inline void set( ID3D12DescriptorHeap* in_heap ) { m_heap = in_heap; }

	inline operator ID3D12DescriptorHeap*( ) const { return m_heap; }
	inline ID3D12DescriptorHeap* operator->( ) const { return m_heap; }

private:
	ID3D12DescriptorHeap* m_heap;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_DESCRIPTOR_HEAP_H_INCLUDED