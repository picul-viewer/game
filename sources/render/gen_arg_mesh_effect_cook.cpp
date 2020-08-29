#include "gen_arg_mesh_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "shader_cook.h"

namespace render {

gen_arg_mesh_effect_cook::gen_arg_mesh_effect_cook( pipeline_state* const in_ptr, type const in_type ) :
	m_result( in_ptr ),
	m_type( in_type )
{ }

void gen_arg_mesh_effect_cook::create_resource( )
{
	shader_define defines[1];
	defines[0].name = "GEN_ARG_TYPE";
	defines[0].value = format( "%d", m_type );

	shader_cook* cook = create_cook<shader_cook>(
		shader_type_compute,
		"gen_arg.cs",
		(u32)array_size( defines ),
		defines
	);

	create_child_resources(
		resource_system::user_callback_task<
			gen_arg_mesh_effect_cook,
			&gen_arg_mesh_effect_cook::on_shaders_ready
		>( this ),
		cook
	);
}

void gen_arg_mesh_effect_cook::on_shaders_ready( queried_resources& in_resources )
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