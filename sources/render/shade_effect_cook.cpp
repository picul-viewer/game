#include "shade_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "dx.h"
#include "resources.h"
#include "shader_cook.h"

namespace render {

shade_effect_cook* shade_effect_cook::create( pipeline_state* const in_ptr )
{
	shade_effect_cook* const result = std_allocator( ).allocate( sizeof(shade_effect_cook) );
	result->init( );
	result->m_result = in_ptr;
	return result;
}

void shade_effect_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void shade_effect_cook::create_resource( )
{
	shader_cook* vs_cook = shader_cook::create(
		shader_type_vertex,
		"fullscreen.vs",
		0,
		nullptr
	);

	shader_cook* ps_cook = shader_cook::create(
		shader_type_pixel,
		"shade.ps",
		0,
		nullptr
	);

	create_child_resources(
		resource_system::user_callback_task<
			shade_effect_cook,
			&shade_effect_cook::on_shaders_ready
		>( this ),
		vs_cook, ps_cook
	);
}

void shade_effect_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr vs_shader = in_resources.get_resource<raw_data::ptr>( );
	raw_data::ptr ps_shader = in_resources.get_resource<raw_data::ptr>( );

	dx_root_signature::root_parameter root_parameters[7];
	root_parameters[0].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL );
	root_parameters[1].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL );
	root_parameters[2].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL );
	root_parameters[3].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );
	root_parameters[4].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 3, 0, D3D12_SHADER_VISIBILITY_PIXEL );
	root_parameters[5].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 4, 0, D3D12_SHADER_VISIBILITY_PIXEL );

	dx_root_signature::descriptor_range descriptor_ranges[2];
	descriptor_ranges[0].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_images_space, image_srv_count );
	descriptor_ranges[1].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_textures_space, resources::texture_descriptors_count );

	root_parameters[6].create_descriptor_table( descriptor_ranges, (u32)array_size( descriptor_ranges ), D3D12_SHADER_VISIBILITY_PIXEL );

	dx_root_signature::static_sampler samplers[1];
	samplers[0].create(
		0, 0, D3D12_SHADER_VISIBILITY_PIXEL,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		(u32)array_size( root_parameters ), root_parameters,
		(u32)array_size( samplers ), samplers,
		false, true, false, false, false, true, false
	);

	dx_root_signature rs;
	rs.create( rs_cook );

	dx_pipeline_state::graphics_cook ps_cook;
	ps_cook.create( );
	ps_cook.set_root_signature( rs );
	ps_cook.set_shader( shader_type_vertex, vs_shader->data( ), vs_shader->size( ) );
	ps_cook.set_shader( shader_type_pixel, ps_shader->data( ), ps_shader->size( ) );
	ps_cook.set_blend( false );
	ps_cook.set_blend_for_rt( 0, false );
	ps_cook.set_rasterizer_state( D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE );
	ps_cook.set_depth( true );
	ps_cook.set_stencil(
		true, 0x1, 0x0,
		D3D12_COMPARISON_FUNC_EQUAL, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP
	);
	ps_cook.set_input_layout( 0, nullptr );
	ps_cook.set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
	ps_cook.set_rtv_count( 1 );
	ps_cook.set_rtv_format( 0, g_dx.back_buffer_format );
	ps_cook.set_dsv_format( DXGI_FORMAT_D24_UNORM_S8_UINT );

	dx_pipeline_state ps;
	ps.create( ps_cook );

	pipeline_state* const result = pipeline_state::create_resource( m_result, rs, ps );
	finish( result );
}

} // namespace render