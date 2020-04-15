void main(
	in	uint	in_vertex_id : SV_VertexID,
	out	float4	out_position : SV_Position
	
#ifdef OUTPUT_TEXCOORD
	, out	float2	out_texcoord : TEXCOORD0
#endif // #ifdef OUTPUT_TEXCOORD

)
{
	const float x = float( ( in_vertex_id & 2 ) << 1 );
	const float y = float( ( in_vertex_id & 1 ) << 2 );
	out_position = float4( x - 1.0f, y - 1.0f, 0.0f, 1.0f );
	
#ifdef OUTPUT_TEXCOORD
	out_texcoord = float2( x * 0.25f, 1.0f - y * 0.25f );
#endif // #ifdef OUTPUT_TEXCOORD
}