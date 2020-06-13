#include "resolve_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "dx.h"
#include "resources.h"
#include "shader_cook.h"

namespace render {

resolve_effect_cook* resolve_effect_cook::create( pipeline_state* const in_ptr )
{
	resolve_effect_cook* const result = std_allocator( ).allocate( sizeof(resolve_effect_cook) );
	result->init( );
	result->m_result = in_ptr;
	return result;
}

void resolve_effect_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void resolve_effect_cook::create_resource( )
{
	shader_cook* vs_cook = shader_cook::create(
		shader_type_vertex,
		"fullscreen.vs",
		0,
		nullptr
	);

	shader_cook* ps_cook = shader_cook::create(
		shader_type_pixel,
		"resolve.ps",
		0,
		nullptr
	);

	create_child_resources(
		resource_system::user_callback_task<
			resolve_effect_cook,
			&resolve_effect_cook::on_shaders_ready
		>( this ),
		vs_cook, ps_cook
	);
}

void resolve_effect_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr vs_shader = in_resources.get_resource<raw_data::ptr>( );
	raw_data::ptr ps_shader = in_resources.get_resource<raw_data::ptr>( );

	dx_root_signature::root_parameter root_parameters[1];

	dx_root_signature::descriptor_range descriptor_ranges[3];
	descriptor_ranges[0].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_images_space, image_srv_count );
	descriptor_ranges[1].create( D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, resources::hlsl_images_space, image_uav_count );
	descriptor_ranges[2].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_textures_space, resources::texture_descriptors_count );

	root_parameters[0].create_descriptor_table( descriptor_ranges, (u32)array_size( descriptor_ranges ), D3D12_SHADER_VISIBILITY_PIXEL );

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		(u32)array_size( root_parameters ), root_parameters,
		0, nullptr,
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
	ps_cook.set_depth( false );
	ps_cook.set_stencil( false );
	ps_cook.set_input_layout( 0, nullptr );
	ps_cook.set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
	ps_cook.set_rtv_count( 1 );
	ps_cook.set_rtv_format( 0, g_dx.back_buffer_format );
	ps_cook.set_dsv_format( DXGI_FORMAT_UNKNOWN );

	dx_pipeline_state ps;
	ps.create( ps_cook );

	pipeline_state* const result = pipeline_state::create_resource( m_result, rs, ps );
	finish( result );
}

} // namespace render