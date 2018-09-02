#ifndef __shader_common_h_included_
#define __shader_common_h_included_

float4 transform_position( float3 position, float4x4 transform )
{
	return mul( transform, float4( position.xyz, 1.0f ) );
}

#endif // #ifndef __shader_common_h_included_