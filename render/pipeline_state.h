#ifndef __render_pipeline_state_h_included_
#define __render_pipeline_state_h_included_

#include <types.h>
#include <math/vector.h>

#include "dx_include.h"

namespace render {

class depth_stencil_state
{
public:
	struct cook
	{
		void set_depth( bool					in_enable_depth		= false,
						bool					in_write_depth		= false,
						D3D11_COMPARISON_FUNC	in_comparison_func	= D3D11_COMPARISON_ALWAYS );

		void set_stencil( bool					in_enable_stencil					= false,
						  u8					in_stencil_read_mask				= D3D11_DEFAULT_STENCIL_READ_MASK,
						  u8					in_stencil_write_mask				= D3D11_DEFAULT_STENCIL_WRITE_MASK,
						  D3D11_COMPARISON_FUNC	in_front_face_comparison_func		= D3D11_COMPARISON_ALWAYS,
						  D3D11_STENCIL_OP		in_front_face_stencil_fail_op		= D3D11_STENCIL_OP_KEEP,
						  D3D11_STENCIL_OP		in_front_face_stencil_depth_fail_op	= D3D11_STENCIL_OP_KEEP,
						  D3D11_STENCIL_OP		in_front_face_stencil_pass_op		= D3D11_STENCIL_OP_KEEP,
						  D3D11_COMPARISON_FUNC	in_back_face_comparison_func		= D3D11_COMPARISON_ALWAYS,
						  D3D11_STENCIL_OP		in_back_face_stencil_fail_op		= D3D11_STENCIL_OP_KEEP,
						  D3D11_STENCIL_OP		in_back_face_stencil_depth_fail_op	= D3D11_STENCIL_OP_KEEP,
						  D3D11_STENCIL_OP		in_back_face_stencil_pass_op		= D3D11_STENCIL_OP_KEEP );

		D3D11_DEPTH_STENCIL_DESC desc;
	};

	void create( cook const& in_cook );
	void set( ID3D11DepthStencilState* in_state );
	void destroy( );
	
	void bind( u8 in_stencil_ref ) const;

protected:
	ID3D11DepthStencilState*	m_depth_stencil_state;
};

class blend_state
{
public:
	struct cook
	{
		void set_blend( bool in_enable_alpha_to_coverage = false,
						bool in_enable_independent_blend = false );

		void set_blend_for_rt( u32				in_render_target_index,
							   bool				in_enable_blending			= false,
							   D3D11_BLEND		in_source_blend				= D3D11_BLEND_ONE,
							   D3D11_BLEND		in_destination_blend		= D3D11_BLEND_ZERO,
							   D3D11_BLEND_OP	in_blend_op					= D3D11_BLEND_OP_ADD,
							   D3D11_BLEND		in_source_blend_alpha		= D3D11_BLEND_ONE,
							   D3D11_BLEND		in_destination_blend_alpha	= D3D11_BLEND_ZERO,
							   D3D11_BLEND_OP	in_blend_op_alpha			= D3D11_BLEND_OP_ADD,
							   u8				in_color_write_mask			= D3D11_COLOR_WRITE_ENABLE_ALL );

		D3D11_BLEND_DESC desc;
	};

	void create( cook const& in_cook );
	void set( ID3D11BlendState* in_state );
	void destroy( );
	
	void bind( math::float4 const& in_blend_factor, u32 in_sample_mask ) const;

protected:
	ID3D11BlendState*			m_blend_state;
};

class rasterizer_state
{
public:
	struct cook
	{
		void set(	D3D11_FILL_MODE	in_fill_mode				= D3D11_FILL_SOLID,
					D3D11_CULL_MODE	in_cull_mode				= D3D11_CULL_BACK,
					bool			in_front_counter_clockwise	= false,
					s32				in_depth_bias				= 0,
					f32				in_depth_bias_clamp			= 0.0f,
					f32				in_slope_scaled_depth_bias	= 0.0f,
					bool			in_depth_clip_enable		= true,
					bool			in_scissor_enable			= false,
					bool			in_multisample_enable		= false,
					bool			in_antialiased_line_enable	= false );

		D3D11_RASTERIZER_DESC desc;
	};

	void create( cook const& in_cook );
	void set( ID3D11RasterizerState* in_state );
	void destroy( );
	
	void bind( ) const;

protected:
	ID3D11RasterizerState*		m_rasterizer_state;
};

} // namespace render

#endif // #ifndef __render_pipeline_state_h_included_