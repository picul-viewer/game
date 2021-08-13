#ifndef GUARD_RENDER_DX_PIPELINE_STATE_H_INCLUDED
#define GUARD_RENDER_DX_PIPELINE_STATE_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include "dx12.h"
#include "dx_root_signature.h"

namespace render {

enum shader_type
{
	shader_type_vertex = 0,
	shader_type_hull,
	shader_type_domain,
	shader_type_geometry,
	shader_type_pixel,
	shader_type_compute,

	shader_type_count
};

class dx_pipeline_state
{
public:
	struct graphics_cook
	{
		void create( );

		void set_root_signature(
			dx_root_signature const in_signature
		);

		void set_shader(
			shader_type const in_shader_type,
			pcvoid const in_data,
			uptr const in_size
		);

		void set_blend(
			bool in_enable_independent_blend = false,
			bool in_enable_alpha_to_coverage = false
		);

		void set_blend_for_rt(
			u32 const in_render_target_index,
			bool const in_enable_blending = false,
			bool const in_enable_logic_operations = false,
			D3D12_BLEND const in_source_blend = D3D12_BLEND_ONE,
			D3D12_BLEND const in_destination_blend = D3D12_BLEND_ZERO,
			D3D12_BLEND_OP const in_blend_op = D3D12_BLEND_OP_ADD,
			D3D12_BLEND const in_source_blend_alpha = D3D12_BLEND_ONE,
			D3D12_BLEND const in_destination_blend_alpha = D3D12_BLEND_ZERO,
			D3D12_BLEND_OP const in_blend_op_alpha = D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP const in_logic_op = D3D12_LOGIC_OP_NOOP,
			u8 const in_color_write_mask = D3D12_COLOR_WRITE_ENABLE_ALL
		);

		void set_rasterizer_state(
			D3D12_FILL_MODE	const in_fill_mode = D3D12_FILL_MODE_SOLID,
			D3D12_CULL_MODE	const in_cull_mode = D3D12_CULL_MODE_BACK,
			bool const in_front_counter_clockwise = false,
			s32	const in_depth_bias = 0,
			f32	const in_depth_bias_clamp = 0.0f,
			f32	const in_slope_scaled_depth_bias = 0.0f,
			bool const in_depth_clip_enable = true,
			bool const in_multisample_enable = false,
			bool const in_antialiased_line_enable = false,
			bool const in_conservative_rasterization_enabled = false
		);

		void set_depth(
			bool const in_enable_depth = false,
			bool const in_write_depth = false,
			D3D12_COMPARISON_FUNC const in_comparison_func = D3D12_COMPARISON_FUNC_ALWAYS
		);

		void set_stencil(
			bool const in_enable_stencil = false,
			u8 const in_stencil_read_mask = D3D12_DEFAULT_STENCIL_READ_MASK,
			u8 const in_stencil_write_mask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
			D3D12_COMPARISON_FUNC const in_front_face_comparison_func = D3D12_COMPARISON_FUNC_ALWAYS,
			D3D12_STENCIL_OP const in_front_face_stencil_fail_op = D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP const in_front_face_stencil_depth_fail_op = D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP const in_front_face_stencil_pass_op = D3D12_STENCIL_OP_KEEP,
			D3D12_COMPARISON_FUNC const in_back_face_comparison_func = D3D12_COMPARISON_FUNC_ALWAYS,
			D3D12_STENCIL_OP const in_back_face_stencil_fail_op = D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP const in_back_face_stencil_depth_fail_op = D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP const in_back_face_stencil_pass_op = D3D12_STENCIL_OP_KEEP
		);

		void set_input_layout(
			u32 const in_element_count,
			D3D12_INPUT_ELEMENT_DESC const* const in_elements
		);

		void set_primitive_topology(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE const in_topology
		);

		void set_rtv_count(
			u32 const in_count
		);

		void set_rtv_format(
			u32 const in_index,
			DXGI_FORMAT const in_format
		);

		void set_dsv_format(
			DXGI_FORMAT const in_format
		);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
	};

	struct compute_cook
	{
		void create( );

		void set_root_signature(
			dx_root_signature const in_signature
		);

		void set_shader(
			pcvoid const in_data,
			uptr const in_size
		);

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
	};

	void create( graphics_cook const& in_cook );
	void create( compute_cook const& in_cook );
	void destroy( );

	inline void set( ID3D12PipelineState* const in_pso ) { m_pso = in_pso; }

	inline operator ID3D12PipelineState*( ) const { return m_pso; }
	inline ID3D12PipelineState* operator->( ) const { return m_pso; }

private:
	ID3D12PipelineState* m_pso;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_PIPELINE_STATE_H_INCLUDED