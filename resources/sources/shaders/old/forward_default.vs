#include "forward_default.h"

#include "common.h"
#include "vertex_input_mesh.h"

cbuffer cb_per_instance : register( b1 )
{
	float4x4 c_WVP;
};

void main( in vertex_input input, out vertex_output output )
{
	output.tex_coords	= input.tex_coords;
	
	output.position		= transform_position( input.position, c_WVP );
}