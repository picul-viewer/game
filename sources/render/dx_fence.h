#ifndef GUARD_RENDER_DX_FENCE_H_INCLUDED
#define GUARD_RENDER_DX_FENCE_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

class dx_fence
{
public:
	void create( u64 const in_initial_value );
	void destroy( );

	inline void set( ID3D12Fence* in_fence ) { m_fence = in_fence; }

	inline operator ID3D12Fence*( ) const { return m_fence; }
	inline ID3D12Fence* operator->( ) const { return m_fence; }

private:
	ID3D12Fence* m_fence;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_FENCE_H_INCLUDED