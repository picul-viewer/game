#include "shade_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "dx.h"
#include "resources.h"
#include "shader_cook.h"

namespace render {

shade_effect_cook::shade_effect_cook( pipeline_state* const in_ptr ) :
	m_result( in_ptr )
{ }

void shade_effect_cook::create_resource( )
{
	shader_define defines[1];
	defines[0].name = "SUN_SHADOWMAP_DIMENSION";
	defines[0].value = format( "%d", resources::sun_shadowmap_dimension );

	shader_cook* cs_cook = create_cook<shader_cook>(
		shader_type_compute,
		"shade.cs",
		(u32)array_size( defines ),
		defines
	);

	create_child_resources(
		resource_system::user_callback_task<
			shade_effect_cook,
			&shade_effect_cook::on_shaders_ready
		>( this ),
		cs_cook
	);
}

void shade_effect_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr shader = in_resources.get_resource<raw_data::ptr>( );

	dx_root_signature::root_parameter root_parameters[9];
	root_parameters[0].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[1].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 0, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[2].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[3].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 2, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[4].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 3, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[5].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 4, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[6].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 5, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[7].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 6, 0, D3D12_SHADER_VISIBILITY_ALL );

	dx_root_signature::descriptor_range descriptor_ranges[3];
	descriptor_ranges[0].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_images_space, image_srv_count );
	descriptor_ranges[1].create( D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, resources::hlsl_images_space, image_uav_count );
	descriptor_ranges[2].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_textures_space, resources::texture_descriptors_count );

	root_parameters[8].create_descriptor_table( descriptor_ranges, (u32)array_size( descriptor_ranges ), D3D12_SHADER_VISIBILITY_ALL );

	dx_root_signature::static_sampler samplers[1];
	samplers[0].create(
		0, 0, D3D12_SHADER_VISIBILITY_ALL,
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

	dx_pipeline_state::compute_cook ps_cook;
	ps_cook.create( );
	ps_cook.set_root_signature( rs );
	ps_cook.set_shader( shader->data( ), shader->size( ) );

	dx_pipeline_state ps;
	ps.create( ps_cook );

	pipeline_state* const result = pipeline_state::create_resource( m_result, rs, ps );
	finish( result );
}

} // namespace render