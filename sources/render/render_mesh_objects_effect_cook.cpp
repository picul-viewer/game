#include "render_mesh_objects_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "shader_cook.h"

namespace render {

render_mesh_objects_effect_cook* render_mesh_objects_effect_cook::create( pipeline_state* const in_ptr )
{
	render_mesh_objects_effect_cook* const result = std_allocator( ).allocate( sizeof(render_mesh_objects_effect_cook) );
	result->init( );
	result->m_result = in_ptr;
	return result;
}

void render_mesh_objects_effect_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void render_mesh_objects_effect_cook::create_resource( )
{
	shader_cook* vs_cook = shader_cook::create(
		shader_type_vertex,
		"visibility.vs",
		0,
		nullptr
	);

	shader_cook* ps_cook = shader_cook::create(
		shader_type_pixel,
		"visibility.ps",
		0,
		nullptr
	);

	create_child_resources(
		resource_system::user_callback_task<
			render_mesh_objects_effect_cook,
			&render_mesh_objects_effect_cook::on_shaders_ready
		>( this ),
		vs_cook, ps_cook
	);
}

void render_mesh_objects_effect_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr vs_shader = in_resources.get_resource<raw_data::ptr>( );
	raw_data::ptr ps_shader = in_resources.get_resource<raw_data::ptr>( );

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		0, nullptr, 0, nullptr,
		true, true, false, false, false, true, false
	);

	dx_root_signature rs;
	rs.create( rs_cook );

	D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_ID", 0, DXGI_FORMAT_R32_UINT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

	dx_pipeline_state::graphics_cook ps_cook;
	ps_cook.create( );
	ps_cook.set_root_signature( rs );
	ps_cook.set_shader( shader_type_vertex, vs_shader->data( ), vs_shader->size( ) );
	ps_cook.set_shader( shader_type_pixel, ps_shader->data( ), ps_shader->size( ) );
	ps_cook.set_blend( false );
	ps_cook.set_blend_for_rt( 0, false );
	ps_cook.set_rasterizer_state( D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK );
	ps_cook.set_depth( true, true, D3D12_COMPARISON_FUNC_LESS_EQUAL );
	ps_cook.set_stencil(
		true, 0x0, 0x1,
		D3D12_COMPARISON_FUNC_ALWAYS, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_REPLACE
	);
	ps_cook.set_input_layout( (u32)array_size( input_layout ), input_layout );
	ps_cook.set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
	ps_cook.set_rtv_count( 1 );
	ps_cook.set_rtv_format( 0, DXGI_FORMAT_R32G32_UINT );
	ps_cook.set_dsv_format( DXGI_FORMAT_D24_UNORM_S8_UINT );

	dx_pipeline_state ps;
	ps.create( ps_cook );

	pipeline_state* const result = pipeline_state::create_resource( m_result, rs, ps );
	finish( result );
}

} // namespace render