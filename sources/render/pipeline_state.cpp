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

u32 graphics_ps::get_cbv_root_index( shader_type const in_shader_type, u32 const in_register ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 0;
	u32 const root_index = m_root_offsets[offset_index] + in_register;
	ASSERT_CMP( root_index, <, m_root_offsets[offset_index + 1], "binding not found" );
	return root_index;
}

u32 graphics_ps::get_srv_root_index( shader_type const in_shader_type, u32 const in_register ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 1;
	u32 const root_index = m_root_offsets[offset_index] + in_register;
	ASSERT_CMP( root_index, <, m_root_offsets[offset_index + 1], "binding not found" );
	return root_index;
}

u32 graphics_ps::get_uav_root_index( shader_type const in_shader_type, u32 const in_register ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 2;
	u32 const root_index = m_root_offsets[offset_index] + in_register;
	ASSERT_CMP( root_index, <, m_root_offsets[offset_index + 1], "binding not found" );
	return root_index;
}

u32 graphics_ps::get_cbv_table_offset( shader_type const in_shader_type, u32 const in_register ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 2;
	u32 const table_index = m_table_offsets[offset_index] + in_register;
	ASSERT_CMP( table_index, <, m_table_offsets[offset_index + 1], "binding not found" );
	return table_index;
}

u32 graphics_ps::get_srv_table_offset( shader_type const in_shader_type, u32 const in_register ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 0;
	u32 const table_index = m_table_offsets[offset_index] + in_register;
	ASSERT_CMP( table_index, <, m_table_offsets[offset_index + 1], "binding not found" );
	return table_index;
}

u32 graphics_ps::get_uav_table_offset( shader_type const in_shader_type, u32 const in_register ) const
{
	ASSERT( in_shader_type != shader_type_compute );
	u32 const offset_index = in_shader_type * 3 + 1;
	u32 const table_index = m_table_offsets[offset_index] + in_register;
	ASSERT_CMP( table_index, <, m_table_offsets[offset_index + 1], "binding not found" );
	return table_index;
}

u32 graphics_ps::get_table_size( ) const
{
	return m_table_offsets[15];
}

void graphics_ps::set_descriptor_tables( dx_command_list const in_cmd_list, u32 const in_table ) const
{
	u32 root_index = 0;
	u16 table_offset = m_table_offsets[0];

	for ( u32 i = 0; i < 5; ++i )
	{
		u16 const next_table_offset = m_table_offsets[i * 3 + 3];
		u32 const table_size = next_table_offset - table_offset;
		if ( table_size )
			in_cmd_list->SetGraphicsRootDescriptorTable( root_index++, g_resources.get_descriptor_table_handle( in_table + table_offset ) );
	}

	u16 const root_descriptos_start = m_root_offsets[0];
	ASSERT_CMP( root_descriptos_start - root_index, <=, 5 );

	for ( ; root_index < root_descriptos_start; ++root_index )
		in_cmd_list->SetGraphicsRootDescriptorTable( root_index, g_resources.get_common_table_handle( ) );
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

u32 compute_ps::get_cbv_root_index( u32 const in_register ) const
{
	u32 const root_index = m_root_offsets[0] + in_register;
	ASSERT_CMP( root_index, <, m_root_offsets[1], "binding not found" );
	return root_index;
}

u32 compute_ps::get_srv_root_index( u32 const in_register ) const
{
	u32 const root_index = m_root_offsets[1] + in_register;
	ASSERT_CMP( root_index, <, m_root_offsets[2], "binding not found" );
	return root_index;
}

u32 compute_ps::get_uav_root_index( u32 const in_register ) const
{
	u32 const root_index = m_root_offsets[2] + in_register;
	ASSERT_CMP( root_index, <, m_root_offsets[3], "binding not found" );
	return root_index;
}

u32 compute_ps::get_cbv_table_offset( u32 const in_register ) const
{
	u32 const table_index = m_table_offsets[2] + in_register;
	ASSERT_CMP( table_index, <, m_table_offsets[3], "binding not found" );
	return table_index;
}

u32 compute_ps::get_srv_table_offset( u32 const in_register ) const
{
	u32 const table_index = m_table_offsets[0] + in_register;
	ASSERT_CMP( table_index, <, m_table_offsets[1], "binding not found" );
	return table_index;
}

u32 compute_ps::get_uav_table_offset( u32 const in_register ) const
{
	u32 const table_index = m_table_offsets[1] + in_register;
	ASSERT_CMP( table_index, <, m_table_offsets[2], "binding not found" );
	return table_index;
}

u32 compute_ps::get_table_size( ) const
{
	return m_table_offsets[3];
}

void compute_ps::set_descriptor_tables( dx_command_list const in_cmd_list, u32 const in_table ) const
{
	u32 root_index = 0;

	if ( get_table_size( ) )
		in_cmd_list->SetComputeRootDescriptorTable( root_index++, g_resources.get_descriptor_table_handle( in_table ) );

	if ( root_index < m_root_offsets[0] )
		in_cmd_list->SetComputeRootDescriptorTable( root_index++, g_resources.get_common_table_handle( ) );

	ASSERT_CMP( root_index, ==, m_root_offsets[0] );
}


void calculate_rs_dimensions(
	u32& in_root_resource_count, u32& in_table_resource_count, u32& in_sampler_count, bool& in_need_common_table,
	ID3D12ShaderReflection* const in_refl, u32 const in_bound_resources
)
{
	u32 root_resource_count = 0;
	u32 table_resource_count = 0;
	u32 sampler_count = 0;
	bool need_common_table = false;

	for ( u32 i = 0; i < in_bound_resources; ++i )
	{
		D3D12_SHADER_INPUT_BIND_DESC binding_desc;
		in_refl->GetResourceBindingDesc( i, &binding_desc );

		if ( binding_desc.Type == D3D_SIT_SAMPLER )
		{
			// Only support root samplers until got problems.
			++sampler_count;
			continue;
		}

		switch ( binding_desc.Space )
		{
			case resources::hlsl_root_descriptors_space:
				ASSERT( binding_desc.BindCount == 1, "implementation needs to be revisited" );
				++root_resource_count;
				break;
			case resources::hlsl_table_descriptors_space:
				ASSERT( binding_desc.BindCount == 1, "implementation needs to be revisited" );
				++table_resource_count;
				break;
			case resources::hlsl_textures_space:
				need_common_table = true;
				break;
			default:
				UNREACHABLE_CODE
		}
	}

	in_root_resource_count = root_resource_count;
	in_table_resource_count = table_resource_count;
	in_sampler_count = sampler_count;
	in_need_common_table = need_common_table;
}

u32 get_root_bind_type_index( D3D_SHADER_INPUT_TYPE const in_type )
{
	switch ( in_type )
	{
		case D3D_SIT_CBUFFER:
			return 0;
        case D3D_SIT_TBUFFER:
		case D3D_SIT_STRUCTURED:
			return 1;
		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_BYTEADDRESS:
		case D3D_SIT_UAV_RWBYTEADDRESS:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			return 2;
		default: UNREACHABLE_CODE
	}

	return -1;
}

u32 get_table_bind_type_index( D3D_SHADER_INPUT_TYPE const in_type )
{
	switch ( in_type )
	{
        case D3D_SIT_TBUFFER:
		case D3D_SIT_TEXTURE:
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
			return 0;
		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_RWBYTEADDRESS:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			return 1;
		case D3D_SIT_CBUFFER:
			return 2;
		default: UNREACHABLE_CODE
	}

	return -1;
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
	u32 const in_root_resource_count, u16* const in_root_binding_offsets, u16* const in_table_binding_offsets,
	dx_root_signature::root_parameter* const in_root_parameters, u32& in_root_parameters_offset,
	dx_root_signature::descriptor_range* const in_table_ranges, u32& in_table_ranges_offset,
	dx_root_signature::static_sampler* const in_samplers,
	D3D12_SHADER_VISIBILITY const in_visibility
)
{
	enum {
		c_root_binding_type_offset = 30,
		c_root_binding_index_mask = 0x3FFFFFFF
	};

	u32* const root_data = (u32*)alloca( in_root_resource_count * sizeof(u32) );

	u32 root_binding_types_count[3] = { };
	u32 table_binding_types_count[3] = { };
	u32 table_binding_max_indices[3] = { 0, 0, 0 };
	u32 root_binding_index = 0;
	u32 sampler_index = 0;

	for ( u32 i = 0; i < in_bindings_count; ++i )
	{
		D3D12_SHADER_INPUT_BIND_DESC binding_desc;
		in_refl->GetResourceBindingDesc( i, &binding_desc );

		if ( binding_desc.Type == D3D_SIT_SAMPLER )
		{
			get_sampler_for_binding( in_samplers[sampler_index++], binding_desc, in_visibility );
			continue;
		}

		switch ( binding_desc.Space )
		{
			case resources::hlsl_root_descriptors_space:
			{
				u32 const bind_type_index = get_root_bind_type_index( binding_desc.Type );
				++root_binding_types_count[bind_type_index];
				u32 const root_binding = ( bind_type_index << c_root_binding_type_offset ) | binding_desc.BindPoint;
				root_data[root_binding_index++] = root_binding;
				break;
			}
			case resources::hlsl_table_descriptors_space:
			{
				u32 const bind_index = get_table_bind_type_index( binding_desc.Type );
				++table_binding_types_count[bind_index];
				table_binding_max_indices[bind_index] = math::max( table_binding_max_indices[bind_index], binding_desc.BindPoint );
				break;
			}
			default:
				break;
		}
	}

	{
		pcstr const c_type_strings[] = { "SRV", "UAV", "CBV" };

		for ( u32 i = 0; i < array_size( c_type_strings ); ++i )
			ASSERT( table_binding_max_indices[i] == math::max( 1u, table_binding_types_count[i] ) - 1,
				"%s bindings registers are not tightly packed", c_type_strings[i] );
	}

	lib::sort( root_data, root_data + in_root_resource_count );

	{
		D3D12_ROOT_PARAMETER_TYPE const c_types[] = { D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_ROOT_PARAMETER_TYPE_SRV, D3D12_ROOT_PARAMETER_TYPE_UAV };

		u16 curr_offset = in_root_binding_offsets[0];

		for ( u32 i = 0; i < array_size( c_types ); ++i )
		{
			curr_offset += root_binding_types_count[i];
			in_root_binding_offsets[i + 1] = curr_offset;
		}

		for ( u32 i = 0; i < in_root_resource_count; ++i )
		{
			in_root_parameters[in_root_parameters_offset + i].create_descriptor( c_types[root_data[i] >> c_root_binding_type_offset],
				root_data[i] & c_root_binding_index_mask, resources::hlsl_root_descriptors_space, in_visibility );
		}

		in_root_parameters_offset += in_root_resource_count;
	}

	{
		D3D12_DESCRIPTOR_RANGE_TYPE const c_types[] = { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, D3D12_DESCRIPTOR_RANGE_TYPE_CBV };

		u16 curr_offset = in_table_binding_offsets[0];
		u32 table_ranges_offset = in_table_ranges_offset;

		for ( u32 j = 0; j < array_size( c_types ); ++j )
		{
			u32 const count = table_binding_types_count[j];

			if ( count != 0 )
				in_table_ranges[table_ranges_offset++].create( c_types[j], 0, resources::hlsl_table_descriptors_space, count );

			curr_offset += (u16)count;
			in_table_binding_offsets[j + 1] = curr_offset;
			/// NOT TIGHTLY PACKED!!!
		}

		in_table_ranges_offset = table_ranges_offset;
	}
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

	u32 all_root_count = 0;
	u32 all_table_count = 0;
	u32 all_sampler_count = 0;
	u32 root_count[5], table_count[5], sampler_count[5];
	bool need_common_table[5];

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

			calculate_rs_dimensions( root_count[i], table_count[i], sampler_count[i], need_common_table[i], reflections[i], bound_resources[i] );
			all_root_count += root_count[i] + ( table_count[i] ? 1 : 0 ) + ( need_common_table[i] ? 1 : 0 );
			all_table_count += table_count[i];
			all_sampler_count += sampler_count[i];
		}
	}

	dx_root_signature::descriptor_range descriptor_ranges[3];
	dx_root_signature::root_parameter* const rs_params = (dx_root_signature::root_parameter*)alloca( all_root_count * sizeof(dx_root_signature::root_parameter) );
	dx_root_signature::static_sampler* const rs_sampler_params = (dx_root_signature::static_sampler*)alloca( all_sampler_count * sizeof(dx_root_signature::static_sampler) );

	u32 root_parameters_offset = 0;

	u32 table_root_parameter_indices[5];
	for ( u32 i = 0; i < 5; ++i )
	{
		table_root_parameter_indices[i] = root_parameters_offset;
		root_parameters_offset += ( m_shader_cooks[i] && table_count[i] ) ? 1 : 0;
	}

	for ( u32 i = 0; i < 5; ++i )
	{
		if ( m_shader_cooks[i] && need_common_table[i] )
			g_resources.serialize_common_descriptor_table_binding( rs_params[root_parameters_offset++],
				(D3D12_SHADER_VISIBILITY)( D3D12_SHADER_VISIBILITY_VERTEX + i ) );
	}

	m_result->m_root_offsets[0] = (u16)root_parameters_offset;
	m_result->m_table_offsets[0] = 0;

	u32 samplers_offset = 0;

	for ( u32 i = 0; i < 5; ++i )
	{
		if ( m_shader_cooks[i] )
		{
			u32 descriptor_ranges_offset = 0;

			serialize_rs_parameters(
				bound_resources[i], reflections[i],
				root_count[i], m_result->m_root_offsets + i * 3, m_result->m_table_offsets + i * 3,
				rs_params, root_parameters_offset,
				descriptor_ranges, descriptor_ranges_offset,
				rs_sampler_params + samplers_offset,
				(D3D12_SHADER_VISIBILITY)( D3D12_SHADER_VISIBILITY_VERTEX + i )
			);

			if ( table_count[i] )
				rs_params[table_root_parameter_indices[i]].create_descriptor_table( descriptor_ranges,
					descriptor_ranges_offset, (D3D12_SHADER_VISIBILITY)( D3D12_SHADER_VISIBILITY_VERTEX + i ) );

			samplers_offset += sampler_count[i];
		}
		else
		{
			m_result->m_root_offsets[i * 3 + 1] = m_result->m_root_offsets[i * 3];
			m_result->m_root_offsets[i * 3 + 2] = m_result->m_root_offsets[i * 3];
			m_result->m_root_offsets[i * 3 + 3] = m_result->m_root_offsets[i * 3];
			m_result->m_table_offsets[i * 3 + 1] = m_result->m_table_offsets[i * 3];
			m_result->m_table_offsets[i * 3 + 2] = m_result->m_table_offsets[i * 3];
			m_result->m_table_offsets[i * 3 + 3] = m_result->m_table_offsets[i * 3];
		}
	}

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		all_root_count, rs_params,
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

	u32 root_count, table_count, sampler_count;
	bool need_common_table;

	calculate_rs_dimensions( root_count, table_count, sampler_count, need_common_table, reflection, shader_desc.BoundResources );

	u32 const rs_params_count = root_count + ( table_count ? 1 : 0 ) + ( need_common_table ? 1 : 0 );
	dx_root_signature::descriptor_range descriptor_ranges[3];
	dx_root_signature::root_parameter* const rs_params = (dx_root_signature::root_parameter*)alloca( rs_params_count * sizeof(dx_root_signature::root_parameter) );
	dx_root_signature::static_sampler* const rs_sampler_params = (dx_root_signature::static_sampler*)alloca( sampler_count * sizeof(dx_root_signature::static_sampler) );

	u32 root_parameters_offset = 0;
	u32 descriptor_ranges_offset = 0;

	root_parameters_offset += table_count ? 1 : 0;

	if ( need_common_table )
		g_resources.serialize_common_descriptor_table_binding( rs_params[root_parameters_offset++], D3D12_SHADER_VISIBILITY_ALL );

	m_result->m_root_offsets[0] = (u16)root_parameters_offset;
	m_result->m_table_offsets[0] = 0;

	serialize_rs_parameters(
		shader_desc.BoundResources, reflection,
		root_count, m_result->m_root_offsets + 0, m_result->m_table_offsets + 0,
		rs_params, root_parameters_offset,
		descriptor_ranges, descriptor_ranges_offset,
		rs_sampler_params,
		D3D12_SHADER_VISIBILITY_ALL
	);

	if ( table_count )
		rs_params[0].create_descriptor_table( descriptor_ranges, descriptor_ranges_offset, D3D12_SHADER_VISIBILITY_ALL );

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