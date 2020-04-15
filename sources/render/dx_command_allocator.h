#ifndef GUARD_RENDER_DX_COMMAND_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_DX_COMMAND_ALLOCATOR_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

class dx_command_allocator
{
public:
	void create( D3D12_COMMAND_LIST_TYPE const in_type );
	void destroy( );

	inline void set( ID3D12CommandAllocator* in_allocator ) { m_allocator = in_allocator; }

	inline operator ID3D12CommandAllocator*( ) const { return m_allocator; }
	inline ID3D12CommandAllocator* operator->( ) const { return m_allocator; }

private:
	ID3D12CommandAllocator* m_allocator;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_COMMAND_ALLOCATOR_H_INCLUDED