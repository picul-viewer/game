#include "pipeline_state.h"
#include "resources.h"
#include <lib/allocator.h>
#include <resource_system/api.h>

#include <d3dcompiler.h>

namespace render {

void graphics_ps::destroy( )
{
	m_ps.destroy( );
	m_rs.destroy( );
}

void graphics_ps::bind_cbv( shader_type const in_shader_type, dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 0;
	u32 const root_index = m_offsets[offset_index] + in_register;
	ASSERT_CMP( root_index, <, m_offsets[offset_index + 1], "binding not found" );
	in_cmd_list->SetGraphicsRootConstantBufferView( root_index, in_address );
}

void graphics_ps::bind_srv( shader_type const in_shader_type, dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 1;
	u32 const root_index = m_offsets[offset_index] + in_register;
	ASSERT_CMP( root_index, <, m_offsets[offset_index + 1], "binding not found" );
	in_cmd_list->SetGraphicsRootShaderResourceView( root_index, in_address );
}

void graphics_ps::bind_uav( shader_type const in_shader_type, dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 2;
	u32 const root_index = m_offsets[offset_index] + in_register;
	ASSERT_CMP( root_index, <, m_offsets[offset_index + 1], "binding not found" );
	in_cmd_list->SetGraphicsRootUnorderedAccessView( root_index, in_address );
}

void graphics_ps::set_descriptor_table( dx_command_list const in_cmd_list, D3D12_GPU_DESCRIPTOR_HANDLE const in_table ) const
{
	for ( u32 i = 0; i < m_offsets[0]; ++i )
		in_cmd_list->SetGraphicsRootDescriptorTable( i, in_table );
}


void compute_ps::destroy( )
{
	m_ps.destroy( );
	m_rs.destroy( );
}

u32 compute_ps::calculate_groups_count( u32 const in_dimensions )
{
	ASSERT( m_group_dims.y == 1 );
	ASSERT( m_group_dims.z == 1 );
	return ( in_dimensions + m_group_dims.x - 1 ) / m_group_dims.x;
}

math::u32x2 compute_ps::calculate_groups_count( math::u32x2 const in_dimensions )
{
	ASSERT( m_group_dims.z == 1 );
	return ( in_dimensions + math::u32x2( m_group_dims ) - math::u32x2( 1 ) ) / math::u32x2( m_group_dims );
}

math::u32x3 compute_ps::calculate_groups_count( math::u32x3 const in_dimensions )
{
	return ( in_dimensions + math::u32x3( m_group_dims ) - math::u32x3( 1 ) ) / math::u32x3( m_group_dims );
}

void compute_ps::bind_cbv( dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const
{
	u32 const root_index = m_offsets[0] + in_register;
	ASSERT_CMP( root_index, <, m_offsets[1], "binding not found" );
	in_cmd_list->SetComputeRootConstantBufferView( root_index, in_address );
}

void compute_ps::bind_srv( dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const
{
	u32 const root_index = m_offsets[1] + in_register;
	ASSERT_CMP( root_index, <, m_offsets[2], "binding not found" );
	in_cmd_list->SetComputeRootShaderResourceView( root_index, in_address );
}

void compute_ps::bind_uav( dx_command_list const in_cmd_list, u32 const in_register, D3D12_GPU_VIRTUAL_ADDRESS const in_address ) const
{
	u32 const root_index = m_offsets[2] + in_register;
	ASSERT_CMP( root_index, <, m_offsets[3], "binding not found" );
	in_cmd_list->SetComputeRootUnorderedAccessView( root_index, in_address );
}

void compute_ps::set_descriptor_table( dx_command_list const in_cmd_list, D3D12_GPU_DESCRIPTOR_HANDLE const in_table ) const
{
	if ( m_offsets[0] )
		in_cmd_list->SetComputeRootDescriptorTable( 0, in_table );
}


void calculate_rs_dimensions(
	u32& in_resource_count, u32& in_sampler_count, bool& need_descriptor_table,
	ID3D12ShaderReflection* const in_refl, u32 const in_bound_resources
)
{
	u32 resource_count = 0;
	u32 sampler_count = 0;

	for ( u32 i = 0; i < in_bound_resources; ++i )
	{
		D3D12_SHADER_INPUT_BIND_DESC binding_desc;
		in_refl->GetResourceBindingDesc( i, &binding_desc );

		bool const is_root_resource = ( binding_desc.Space < resources::hlsl_table_space_start ) && ( binding_desc.Type != D3D_SIT_SAMPLER );
		resource_count += is_root_resource ? 1 : 0;

		// Only support root samplers until got problems.
		bool const is_sampler = binding_desc.Type == D3D_SIT_SAMPLER;
		sampler_count += is_sampler ? 1 : 0;
	}

	in_resource_count = resource_count;
	in_sampler_count = sampler_count;
	need_descriptor_table = resource_count + sampler_count < in_bound_resources;
}

D3D12_ROOT_PARAMETER_TYPE get_bind_type( D3D_SHADER_INPUT_TYPE const in_type )
{
	switch ( in_type )
	{
		case D3D_SIT_CBUFFER:
			return D3D12_ROOT_PARAMETER_TYPE_CBV;
        case D3D_SIT_TBUFFER:
		case D3D_SIT_STRUCTURED:
			return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_BYTEADDRESS:
		case D3D_SIT_UAV_RWBYTEADDRESS:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			return D3D12_ROOT_PARAMETER_TYPE_UAV;
		default: UNREACHABLE_CODE
	}

	return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
}

void get_sampler_for_binding( dx_root_signature::static_sampler& in_sampler_desc, D3D12_SHADER_INPUT_BIND_DESC const& in_binding_desc, D3D12_SHADER_VISIBILITY const in_visibility )
{
	if ( strings::equal( in_binding_desc.Name, "g_texture_sampler" ) )
	{
		in_sampler_desc.create(
			in_binding_desc.BindPoint, in_binding_desc.Space, in_visibility,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		);
		return;
	}

	if ( strings::equal( in_binding_desc.Name, "g_ui_sampler" ) )
	{
		in_sampler_desc.create(
			in_binding_desc.BindPoint, in_binding_desc.Space, in_visibility,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		);
		return;
	}

	UNREACHABLE_CODE
}

void serialize_rs_parameters(
	u32 const in_bindings_count, ID3D12ShaderReflection* const in_refl,
	u32 const in_resource_count, u32 const in_sampler_count,
	u16* const in_binding_offsets,
	dx_root_signature::root_parameter* const in_parameters, u32 const in_parameters_offset,
	dx_root_signature::static_sampler* const in_samplers, u32 const in_samplers_offset,
	D3D12_SHADER_VISIBILITY const in_visibility
)
{

	struct binding_data
	{
		D3D12_ROOT_PARAMETER_TYPE type;
		u32 index;
	}* data = (binding_data*)alloca( in_resource_count * sizeof(binding_data) );

	u32 binding_index = 0;
	u32 sampler_index = 0;

	for ( u32 i = 0; i < in_bindings_count; ++i )
	{
		D3D12_SHADER_INPUT_BIND_DESC binding_desc;
		in_refl->GetResourceBindingDesc( i, &binding_desc );

		bool const is_root_resource = ( binding_desc.Space < resources::hlsl_table_space_start ) && ( binding_desc.Type != D3D_SIT_SAMPLER );
		bool const is_sampler = binding_desc.Type == D3D_SIT_SAMPLER;

		if ( is_root_resource )
		{
			ASSERT( binding_desc.Space == 0, "custom bindings should be in space0" );
			data[binding_index].type = get_bind_type( binding_desc.Type );
			data[binding_index].index = binding_desc.BindPoint;
			++binding_index;
		}
		else if ( is_sampler )
		{
			get_sampler_for_binding( in_samplers[in_samplers_offset + sampler_index++], binding_desc, in_visibility );
		}
	}

	lib::sort( data, data + in_resource_count, []( binding_data const& l, binding_data const& r )
	{
		if ( l.type != r.type )
			return l.type < r.type;
		return l.index < r.index;
	} );

	D3D12_ROOT_PARAMETER_TYPE const types[] = { D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_ROOT_PARAMETER_TYPE_SRV, D3D12_ROOT_PARAMETER_TYPE_UAV };

	u32 i = 0;

	for ( u32 j = 0; j < 3; ++j )
	{
		in_binding_offsets[j] = in_parameters_offset + i;
		while ( data[i].type == types[j] )
		{
			ASSERT( in_parameters_offset + i - in_binding_offsets[j] == data[i].index, "bindings' registers should be compacted" );
			in_parameters[in_parameters_offset + i].create_descriptor( data[i].type, data[i].index, 0, in_visibility );
			++i;
		}
	}
}

void serialize_descriptor_table_binding( dx_root_signature::root_parameter& in_parameter, D3D12_SHADER_VISIBILITY const in_visibility )
{
	static struct descriptor_ranges
	{
		dx_root_signature::descriptor_range data[3];

		descriptor_ranges( )
		{
			data[0].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_images_space, image_srv_count );
			data[1].create( D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, resources::hlsl_images_space, image_uav_count );
			data[2].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_textures_space, resources::texture_descriptors_count );
		}
	} ranges;

	in_parameter.create_descriptor_table( ranges.data, (u32)array_size( ranges.data ), in_visibility );
}


graphics_ps_cook::graphics_ps_cook( graphics_ps* const in_ptr ) :
	m_result( in_ptr )
{
	m_desc.create( );
	m_shader_cooks[0] = nullptr;
	m_shader_cooks[1] = nullptr;
	m_shader_cooks[2] = nullptr;
	m_shader_cooks[3] = nullptr;
	m_shader_cooks[4] = nullptr;
}

void graphics_ps_cook::set_blend(
	bool in_enable_independent_blend,
	bool in_enable_alpha_to_coverage
)
{
	m_desc.set_blend(
		in_enable_alpha_to_coverage,
		in_enable_alpha_to_coverage
	);
}

void graphics_ps_cook::set_blend_for_rt(
	u32 const in_render_target_index,
	bool const in_enable_blending,
	bool const in_enable_logic_operations,
	D3D12_BLEND const in_source_blend,
	D3D12_BLEND const in_destination_blend,
	D3D12_BLEND_OP const in_blend_op,
	D3D12_BLEND const in_source_blend_alpha,
	D3D12_BLEND const in_destination_blend_alpha,
	D3D12_BLEND_OP const in_blend_op_alpha,
	D3D12_LOGIC_OP const in_logic_op,
	u8 const in_color_write_mask
)
{
	m_desc.set_blend_for_rt(
		in_render_target_index,
		in_enable_blending,
		in_enable_logic_operations,
		in_source_blend,
		in_destination_blend,
		in_blend_op,
		in_source_blend_alpha,
		in_destination_blend_alpha,
		in_blend_op_alpha,
		in_logic_op,
		in_color_write_mask
	);
}

void graphics_ps_cook::set_rasterizer_state(
	D3D12_FILL_MODE	const in_fill_mode,
	D3D12_CULL_MODE	const in_cull_mode,
	bool const in_front_counter_clockwise,
	s32	const in_depth_bias,
	f32	const in_depth_bias_clamp,
	f32	const in_slope_scaled_depth_bias,
	bool const in_depth_clip_enable,
	bool const in_multisample_enable,
	bool const in_antialiased_line_enable,
	bool const in_conservative_rasterization_enabled
)
{
	m_desc.set_rasterizer_state(
		in_fill_mode,
		in_cull_mode,
		in_front_counter_clockwise,
		in_depth_bias,
		in_depth_bias_clamp,
		in_slope_scaled_depth_bias,
		in_depth_clip_enable,
		in_multisample_enable,
		in_antialiased_line_enable,
		in_conservative_rasterization_enabled
	);
}

void graphics_ps_cook::set_depth(
	bool const in_enable_depth,
	bool const in_write_depth,
	D3D12_COMPARISON_FUNC const in_comparison_func
)
{
	m_desc.set_depth(
		in_enable_depth,
		in_write_depth,
		in_comparison_func
	);
}

void graphics_ps_cook::set_stencil(
	bool const in_enable_stencil,
	u8 const in_stencil_read_mask,
	u8 const in_stencil_write_mask,
	D3D12_COMPARISON_FUNC const in_front_face_comparison_func,
	D3D12_STENCIL_OP const in_front_face_stencil_fail_op,
	D3D12_STENCIL_OP const in_front_face_stencil_depth_fail_op,
	D3D12_STENCIL_OP const in_front_face_stencil_pass_op,
	D3D12_COMPARISON_FUNC const in_back_face_comparison_func,
	D3D12_STENCIL_OP const in_back_face_stencil_fail_op,
	D3D12_STENCIL_OP const in_back_face_stencil_depth_fail_op,
	D3D12_STENCIL_OP const in_back_face_stencil_pass_op
)
{
	m_desc.set_stencil(
		in_enable_stencil,
		in_stencil_read_mask,
		in_stencil_write_mask,
		in_front_face_comparison_func,
		in_front_face_stencil_fail_op,
		in_front_face_stencil_depth_fail_op,
		in_front_face_stencil_pass_op,
		in_back_face_comparison_func,
		in_back_face_stencil_fail_op,
		in_back_face_stencil_depth_fail_op,
		in_back_face_stencil_pass_op
	);
}

void graphics_ps_cook::set_input_layout( u32 const in_element_count, D3D12_INPUT_ELEMENT_DESC const* const in_elements )
{
	ASSERT_CMP( in_element_count, <, max_input_elements );
	memory::copy( m_input_elements, in_elements, sizeof(D3D12_INPUT_ELEMENT_DESC) * in_element_count );
	m_desc.set_input_layout( in_element_count, m_input_elements );
}

void graphics_ps_cook::set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE const in_topology )
{
	m_desc.set_primitive_topology( in_topology );
}

void graphics_ps_cook::set_rtv_count( u32 const in_count )
{
	m_desc.set_rtv_count( in_count );
}

void graphics_ps_cook::set_rtv_format( u32 const in_index, DXGI_FORMAT const in_format )
{
	m_desc.set_rtv_format( in_index, in_format );
}

void graphics_ps_cook::set_dsv_format( DXGI_FORMAT const in_format )
{
	m_desc.set_dsv_format( in_format );
}

void graphics_ps_cook::create_resource( )
{
	task_info tasks[5];
	u32 count = 0;

	ASSERT( m_shader_cooks[0] );
	m_shader_cooks[0]->fill_task_info( tasks[count++] );

	for ( u32 i = 1; i < 5; ++i )
	{
		if ( m_shader_cooks[i] )
			m_shader_cooks[i]->fill_task_info( tasks[count++] );
	}

	create_child_resources(
		tasks, count,
		resource_system::user_callback_task<
			graphics_ps_cook,
			&graphics_ps_cook::on_shaders_ready
		>( this )
	);
}

void graphics_ps_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr shaders[5];
	ID3D12ShaderReflection* reflections[5];
	u32 bound_resources[5];

	u32 all_bindings_count = 0;
	u32 all_sampler_count = 0;
	u32 resource_count[5], sampler_count[5];
	bool need_descriptor_table[5];

	for ( u32 i = 0; i < 5; ++i )
	{
		if ( m_shader_cooks[i] )
		{
			shaders[i] = in_resources.get_resource<raw_data::ptr>( );
			m_desc.set_shader( (shader_type)( shader_type_vertex + i ), shaders[i]->data( ), shaders[i]->size( ) );

			DX12_CHECK_RESULT( D3DReflect( shaders[i]->data( ), shaders[i]->size( ), IID_PPV_ARGS( &reflections[i] ) ) );
			D3D12_SHADER_DESC desc;
			DX12_CHECK_RESULT( reflections[i]->GetDesc( &desc ) );
			bound_resources[i] = desc.BoundResources;

			calculate_rs_dimensions( resource_count[i], sampler_count[i], need_descriptor_table[i], reflections[i], bound_resources[i] );
			all_bindings_count += resource_count[i] + ( need_descriptor_table[i] ? 1 : 0 );
			all_sampler_count += sampler_count[i];
		}
	}

	dx_root_signature::root_parameter* const rs_params = (dx_root_signature::root_parameter*)alloca( all_bindings_count * sizeof(dx_root_signature::root_parameter) );
	dx_root_signature::static_sampler* const rs_sampler_params = (dx_root_signature::static_sampler*)alloca( all_sampler_count * sizeof(dx_root_signature::static_sampler) );

	u32 bindings_offset = 0;
	u32 samplers_offset = 0;

	for ( u32 i = 0; i < 5; ++i )
	{
		if ( m_shader_cooks[i] && need_descriptor_table[i] )
			serialize_descriptor_table_binding( rs_params[bindings_offset++], (D3D12_SHADER_VISIBILITY)( D3D12_SHADER_VISIBILITY_VERTEX + i ) );
	}

	for ( u32 i = 0; i < 5; ++i )
	{
		if ( m_shader_cooks[i] )
		{
			serialize_rs_parameters(
				bound_resources[i], reflections[i],
				resource_count[i], sampler_count[i], m_result->m_offsets + i * 3,
				rs_params, bindings_offset,
				rs_sampler_params, samplers_offset,
				(D3D12_SHADER_VISIBILITY)( D3D12_SHADER_VISIBILITY_VERTEX + i )
			);

			bindings_offset += resource_count[i];
			samplers_offset += sampler_count[i];
		}
		else
		{
			m_result->m_offsets[i * 3 + 0] = m_result->m_offsets[i * 3 - 1];
			m_result->m_offsets[i * 3 + 1] = m_result->m_offsets[i * 3 - 1];
			m_result->m_offsets[i * 3 + 2] = m_result->m_offsets[i * 3 - 1];
		}
	}

	m_result->m_offsets[15] = bindings_offset;

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		all_bindings_count, rs_params,
		all_sampler_count, rs_sampler_params,
		m_desc.desc.InputLayout.NumElements != 0,
		true, m_shader_cooks[1], m_shader_cooks[2], m_shader_cooks[3], m_shader_cooks[4], false
	);

	dx_root_signature rs;
	rs.create( rs_cook );

	dx_pipeline_state ps;
	m_desc.set_root_signature( rs );
	ps.create( m_desc );

	m_result->m_rs = rs;
	m_result->m_ps = ps;
	finish( m_result );
}


compute_ps_cook::compute_ps_cook( compute_ps* const in_ptr, shader_cook* in_shader ) :
	m_result( in_ptr ),
	m_cs( in_shader )
{ }

void compute_ps_cook::create_resource( )
{
	ASSERT( m_cs );

	create_child_resources(
		resource_system::user_callback_task<
			compute_ps_cook,
			&compute_ps_cook::on_shader_ready
		>( this ),
		m_cs
	);
}

void compute_ps_cook::on_shader_ready( queried_resources& in_resources )
{
	raw_data::ptr shader = in_resources.get_resource<raw_data::ptr>( );

	ID3D12ShaderReflection* reflection;
	DX12_CHECK_RESULT( D3DReflect( shader->data( ), shader->size( ), IID_PPV_ARGS( &reflection ) ) );
	D3D12_SHADER_DESC shader_desc;
	DX12_CHECK_RESULT( reflection->GetDesc( &shader_desc ) );

	math::u32x3 group_dimensions;
	u32 const group_size = reflection->GetThreadGroupSize( &group_dimensions.x, &group_dimensions.y, &group_dimensions.z );

	m_result->m_group_dims = math::u16x4( group_dimensions.x, group_dimensions.y, group_dimensions.z, group_size );

	u32 resource_count, sampler_count;
	bool need_descriptor_table;

	calculate_rs_dimensions( resource_count, sampler_count, need_descriptor_table, reflection, shader_desc.BoundResources );

	u32 const rs_params_count = resource_count + ( need_descriptor_table ? 1 : 0 );
	dx_root_signature::root_parameter* const rs_params = (dx_root_signature::root_parameter*)alloca( rs_params_count * sizeof(dx_root_signature::root_parameter) );
	dx_root_signature::static_sampler* const rs_sampler_params = (dx_root_signature::static_sampler*)alloca( sampler_count * sizeof(dx_root_signature::static_sampler) );

	if ( need_descriptor_table )
		serialize_descriptor_table_binding( rs_params[0], D3D12_SHADER_VISIBILITY_ALL );

	serialize_rs_parameters(
		shader_desc.BoundResources, reflection,
		resource_count, sampler_count, m_result->m_offsets + 0,
		rs_params, need_descriptor_table ? 1 : 0,
		rs_sampler_params, 0,
		D3D12_SHADER_VISIBILITY_ALL
	);

	m_result->m_offsets[3] = rs_params_count;

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		rs_params_count, rs_params,
		sampler_count, rs_sampler_params,
		false, false, false, false, false, false, false
	);

	dx_root_signature rs;
	rs.create( rs_cook );

	dx_pipeline_state::compute_cook ps_cook;
	ps_cook.create( );
	ps_cook.set_root_signature( rs );
	ps_cook.set_shader( shader->data( ), shader->size( ) );

	dx_pipeline_state ps;
	ps.create( ps_cook );

	m_result->m_rs = rs;
	m_result->m_ps = ps;
	finish( m_result );
}

} // namespace render