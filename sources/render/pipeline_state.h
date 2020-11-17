#ifndef GUARD_RENDER_PIPELINE_STATE_H_INCLUDED
#define GUARD_RENDER_PIPELINE_STATE_H_INCLUDED

#include <types.h>
#include <resource_system/default_resource.h>
#include <resource_system/default_resource_cook.h>
#include <utils/engine_threads.h>
#include "dx_command_list.h"
#include "dx_pipeline_state.h"
#include "shader_cook.h"

namespace render {

class graphics_ps : public default_resource<graphics_ps>
{
public:
	friend class graphics_ps_cook;
	friend class command_list_builder;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	void destroy( );

private:
	inline dx_root_signature const& rs( ) const { return m_rs; }
	inline dx_pipeline_state const& ps( ) const { return m_ps; }

	void bind_cbv( shader_type const in_shader_type, dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const;
	void bind_srv( shader_type const in_shader_type, dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const;
	void bind_uav( shader_type const in_shader_type, dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const;

	void set_descriptor_table( dx_command_list const in_cmd_list, D3D12_GPU_DESCRIPTOR_HANDLE const in_table ) const;

private:
	dx_root_signature m_rs;
	dx_pipeline_state m_ps;
	u16 m_offsets[16];

};

class compute_ps : public default_resource<compute_ps>
{
public:
	friend class compute_ps_cook;
	friend class command_list_builder;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	void destroy( );

private:
	inline dx_root_signature const& rs( ) const { return m_rs; }
	inline dx_pipeline_state const& ps( ) const { return m_ps; }

	void bind_cbv( dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const;
	void bind_srv( dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const;
	void bind_uav( dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const;

	void set_descriptor_table( dx_command_list const in_cmd_list, D3D12_GPU_DESCRIPTOR_HANDLE in_table ) const;

private:
	dx_root_signature m_rs;
	dx_pipeline_state m_ps;
	u16 m_offsets[4];

};

class graphics_ps_cook : public default_resource_cook<graphics_ps, graphics_ps_cook>
{
public:
	graphics_ps_cook( graphics_ps* const in_ptr );

	inline void set_vs( shader_cook* in_shader ) { m_shader_cooks[0] = in_shader; }
	inline void set_hs( shader_cook* in_shader ) { m_shader_cooks[1] = in_shader; }
	inline void set_ds( shader_cook* in_shader ) { m_shader_cooks[2] = in_shader; }
	inline void set_gs( shader_cook* in_shader ) { m_shader_cooks[3] = in_shader; }
	inline void set_ps( shader_cook* in_shader ) { m_shader_cooks[4] = in_shader; }

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

public:
	enum : u32 {
		create_thread_index = engine_thread_main
	};

	void create_resource( );

private:
	void on_shaders_ready( queried_resources& in_resources );

private:
	enum : u32 { max_input_elements = 8 };

private:
	graphics_ps* m_result;
	shader_cook* m_shader_cooks[5];
	dx_pipeline_state::graphics_cook m_desc;
	D3D12_INPUT_ELEMENT_DESC m_input_elements[max_input_elements];

};

class compute_ps_cook : public default_resource_cook<compute_ps, compute_ps_cook>
{
public:
	compute_ps_cook( compute_ps* const in_ptr, shader_cook* in_shader );

public:
	enum : u32 {
		create_thread_index = engine_thread_main
	};

	void create_resource( );

private:
	void on_shader_ready( queried_resources& in_resources );

private:
	compute_ps* m_result;
	shader_cook* m_cs = nullptr;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_PIPELINE_STATE_H_INCLUDED