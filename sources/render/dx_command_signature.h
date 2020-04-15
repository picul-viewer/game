#ifndef GUARD_RENDER_DX_COMMAND_SIGNATURE_H_INCLUDED
#define GUARD_RENDER_DX_COMMAND_SIGNATURE_H_INCLUDED

#include <types.h>
#include "dx12.h"
#include "dx_root_signature.h"

namespace render {

class dx_command_signature
{
public:
	struct indirect_argument
	{
		void create_draw( );
		void create_draw_indexed( );
		void create_dispatch( );
		void create_vertex_buffer_update( u32 const in_slot );
		void create_index_buffer_update( );
		void create_constants_update(
			u32 const in_root_parameter_index,
			u32 const in_offset,
			u32 const in_count
		);
		void create_cbv_update( u32 const in_root_parameter_index );
		void create_srv_update( u32 const in_root_parameter_index );
		void create_uav_update( u32 const in_root_parameter_index );

		D3D12_INDIRECT_ARGUMENT_DESC desc;
	};

	struct cook
	{
		void create(
			u32 const in_stride,
			u32 const in_argument_count,
			indirect_argument* const in_arguments
		);

		D3D12_COMMAND_SIGNATURE_DESC desc;
		bool need_rs;
	};

	void create( cook const& in_cook, dx_root_signature const& in_root_signature );
	void destroy( );

	inline void set( ID3D12CommandSignature* const in_cs ) { m_command_signature = in_cs; }

	inline operator ID3D12CommandSignature*( ) const { return m_command_signature; }
	inline ID3D12CommandSignature* operator->( ) const { return m_command_signature; }

private:
	ID3D12CommandSignature* m_command_signature;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_COMMAND_SIGNATURE_H_INCLUDED