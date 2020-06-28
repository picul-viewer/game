void main(
	in	float3		in_position		: POSITION,
	in	float4x4	in_transform	: TRANSFORM,
	out	float4		out_position	: SV_Position
)
{
	float4 hdc_position = mul( float4( in_position, 1.0f ), in_transform );

#ifdef DIRECTIONAL_LIGHT
	// Know that last component is 1.0 (calculations should be removed by compiler).
	out_position = float4( hdc_position.xyz, 1.0f );
#else
	out_position = hdc_position;
#endif // #ifdef DIRECTIONAL_LIGHT
}