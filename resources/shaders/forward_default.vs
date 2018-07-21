#include "forward_default.h"

#include "common.h"
#include "vertex_input_mesh.h"

float4 main( in vertex_input input, out vertex_output output ) : SV_Position
{
	output.tex_coords	= input.tex_coords;
	
	return transform_position( input.position, c_WVP );
}