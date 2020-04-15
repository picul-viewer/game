#include "generate_mesh_arguments_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "shader_cook.h"

namespace render {

generate_mesh_arguments_effect_cook* generate_mesh_arguments_effect_cook::create( pipeline_state* const in_ptr )
{
	generate_mesh_arguments_effect_cook* const result = std_allocator( ).allocate( sizeof(generate_mesh_arguments_effect_cook) );
	result->init( );
	result->m_result = in_ptr;
	return result;
}

void generate_mesh_arguments_effect_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void generate_mesh_arguments_effect_cook::create_resource( )
{
	shader_cook* cook = shader_cook::create(
		shader_type_compute,
		"generate_mesh_arguments.comp",
		0,
		nullptr
	);

	create_child_resources(
		resource_system::user_callback_task<
			generate_mesh_arguments_effect_cook,
			&generate_mesh_arguments_effect_cook::on_shaders_ready
		>( this ),
		cook
	);
}

void generate_mesh_arguments_effect_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr shader = in_resources.get_resource<raw_data::ptr>( );

	dx_root_signature::root_parameter root_parameters[6];
	root_parameters[0].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[1].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 0, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[2].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[3].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_SRV, 2, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[4].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_UAV, 0, 0, D3D12_SHADER_VISIBILITY_ALL );
	root_parameters[5].create_descriptor( D3D12_ROOT_PARAMETER_TYPE_UAV, 1, 0, D3D12_SHADER_VISIBILITY_ALL );

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		(u32)array_size( root_parameters ), root_parameters, 0, nullptr
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