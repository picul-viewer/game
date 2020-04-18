#ifndef GUARD_RENDER_DX_QUERY_HEAP_H_INCLUDED
#define GUARD_RENDER_DX_QUERY_HEAP_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

class dx_query_heap
{
public:
	void create( u32 const in_count, D3D12_QUERY_HEAP_TYPE const in_type );
	void destroy( );

	inline void set( ID3D12QueryHeap* in_heap ) { m_heap = in_heap; }

	inline operator ID3D12QueryHeap*( ) const { return m_heap; }
	inline ID3D12QueryHeap* operator->( ) const { return m_heap; }

private:
	ID3D12QueryHeap* m_heap;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_QUERY_HEAP_H_INCLUDED