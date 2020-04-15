void main(
	in	float3		in_position		: POSITION,
	in	float4x4	in_transform	: TRANSFORM,
	in	uint		in_instance_id	: INSTANCE_ID,
	out	float4		out_position	: SV_Position,
	nointerpolation
	out uint		out_instance_id	: INSTANCE_ID
)
{
	out_position = mul( float4( in_position, 1.0f ), in_transform );
	out_instance_id = in_instance_id;
}