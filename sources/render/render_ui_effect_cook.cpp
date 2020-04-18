#include "render_ui_effect_cook.h"
#include <resources/resources_path.h>
#include <resource_system/api.h>
#include "resources.h"
#include "shader_cook.h"

namespace render {

render_ui_effect_cook* render_ui_effect_cook::create( pipeline_state* const in_ptr )
{
	render_ui_effect_cook* const result = std_allocator( ).allocate( sizeof(render_ui_effect_cook) );
	result->init( );
	result->m_result = in_ptr;
	return result;
}

void render_ui_effect_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void render_ui_effect_cook::create_resource( )
{
	shader_cook* vs_cook = shader_cook::create(
		shader_type_vertex,
		"ui.vs",
		0,
		nullptr
	);

	shader_cook* ps_cook = shader_cook::create(
		shader_type_pixel,
		"ui.ps",
		0,
		nullptr
	);

	create_child_resources(
		resource_system::user_callback_task<
			render_ui_effect_cook,
			&render_ui_effect_cook::on_shaders_ready
		>( this ),
		vs_cook, ps_cook
	);
}

void render_ui_effect_cook::on_shaders_ready( queried_resources& in_resources )
{
	raw_data::ptr vs_shader = in_resources.get_resource<raw_data::ptr>( );
	raw_data::ptr ps_shader = in_resources.get_resource<raw_data::ptr>( );

	dx_root_signature::root_parameter root_parameters[1];

	dx_root_signature::descriptor_range descriptor_ranges[2];
	descriptor_ranges[0].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_images_space, image_srv_count );
	descriptor_ranges[1].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_textures_space, resources::texture_descriptors_count );

	root_parameters[0].create_descriptor_table( descriptor_ranges, (u32)array_size( descriptor_ranges ), D3D12_SHADER_VISIBILITY_PIXEL );

	dx_root_signature::static_sampler samplers[1];
	samplers[0].create(
		0, 0, D3D12_SHADER_VISIBILITY_PIXEL,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	dx_root_signature::cook rs_cook;
	rs_cook.create(
		(u32)array_size( root_parameters ), root_parameters,
		(u32)array_size( samplers ), samplers,
		true, true, false, false, false, true, false
	);

	dx_root_signature rs;
	rs.create( rs_cook );

	D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R16G16_SNORM, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 1, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXTURE_ID", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	dx_pipeline_state::graphics_cook ps_cook;
	ps_cook.create( );
	ps_cook.set_root_signature( rs );
	ps_cook.set_shader( shader_type_vertex, vs_shader->data( ), vs_shader->size( ) );
	ps_cook.set_shader( shader_type_pixel, ps_shader->data( ), ps_shader->size( ) );
	ps_cook.set_blend( false );
	ps_cook.set_blend_for_rt(
		0, true, false,
		D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ZERO, D3D12_BLEND_ONE, D3D12_BLEND_OP_ADD );
	ps_cook.set_rasterizer_state( D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE );
	ps_cook.set_depth( false );
	ps_cook.set_stencil( false );
	ps_cook.set_input_layout( (u32)array_size( input_layout ), input_layout );
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