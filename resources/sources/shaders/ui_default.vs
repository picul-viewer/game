#include "ui_default.h"
#include "vertex_input_ui.h"

void main( in vertex_input input, out vertex_output output )
{
	output.position		= float4( input.position, 0.0f, 1.0f );
	output.tex_coords	= input.tex_coords;
	output.mult_color	= input.mult_color;
	output.add_color	= input.add_color;
}