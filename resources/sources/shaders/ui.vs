void main(
	in	float2		in_position		: POSITION,
	in	float2		in_texcoord		: TEXCOORD,
	in	float4		in_mult_color	: COLOR0,
	in	float4		in_add_color	: COLOR1,
	in	uint		in_texture_id	: TEXTURE_ID,
	out	float4		out_position	: SV_Position,
	out	float2		out_texcoord	: TEXCOORD,
	nointerpolation
	out	float4		out_mult_color	: COLOR0,
	nointerpolation
	out	float4		out_add_color	: COLOR1,
	nointerpolation
	out	uint		out_texture_id	: TEXTURE_ID
)
{
	out_position		= float4( in_position, 0.0f, 1.0f );
	out_texcoord		= in_texcoord;
	out_mult_color		= in_mult_color;
	out_add_color		= in_add_color;
	out_texture_id		= in_texture_id;
}