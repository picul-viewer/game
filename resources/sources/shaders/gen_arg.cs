#include "structures.h"
#include "common.h"

StructuredBuffer<uint>								g_mesh_ids					: register( t0 );
StructuredBuffer<mesh_object>						g_mesh_objects				: register( t1 );
StructuredBuffer<float4x3>							g_transforms				: register( t2 );
RWStructuredBuffer<draw_indexed_indirect_command>	g_indirect_arguments_buffer	: register( u0 );
RWStructuredBuffer<float4x4>						g_mesh_transform_buffer		: register( u1 );

#define GEN_ARG_TYPE_MESH 0
#define GEN_ARG_TYPE_SUN_SHADOWMAP 1

uint size( )
{
#if GEN_ARG_TYPE == GEN_ARG_TYPE_MESH
	return mesh_list_size( );
#elif GEN_ARG_TYPE == GEN_ARG_TYPE_SUN_SHADOWMAP
	return sun_shadowmap_mesh_list_size( );
#endif // #if GEN_ARG_TYPE == GEN_ARG_TYPE_MESH
}

float4x4 world_to_hdc_transform( )
{
#if GEN_ARG_TYPE == GEN_ARG_TYPE_MESH
	return g_constant_buffer.view_projection;
#elif GEN_ARG_TYPE == GEN_ARG_TYPE_SUN_SHADOWMAP
	return g_constant_buffer.sun_view_projection;
#endif // #if GEN_ARG_TYPE == GEN_ARG_TYPE_MESH
}

[numthreads( 256, 1, 1 )]
void main( uint3 id : SV_DispatchThreadID )
{
	const uint i = id.x;

	if ( i >= size( ) )
		return;

	const mesh_object mesh = g_mesh_objects[g_mesh_ids[i] - 1];

	g_indirect_arguments_buffer[i].index_count_per_instance = mesh.index_count;
	g_indirect_arguments_buffer[i].instance_count = 1;
	g_indirect_arguments_buffer[i].start_index_location = mesh.index_buffer_offset;
	g_indirect_arguments_buffer[i].base_vertex_location = mesh.vertex_buffer_offset;
	g_indirect_arguments_buffer[i].start_instance_location = i;

	const float4x3 local_to_world = g_transforms[mesh.transform_index];
	const float4x4 local_to_world_4x4 = float4x4(
		transpose( local_to_world )[0].xyzw,
		transpose( local_to_world )[1].xyzw,
		transpose( local_to_world )[2].xyzw,
		float4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	const float4x4 view_projection = world_to_hdc_transform( );
	g_mesh_transform_buffer[i] = mul( view_projection, local_to_world_4x4 );
}