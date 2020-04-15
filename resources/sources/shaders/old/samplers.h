#ifndef __shader_samplers_h_included_
#define __shader_samplers_h_included_

// Texture sampler.
SamplerState s_texture_sampler	: register( s0 );

// Point sampler.
SamplerState s_point_sampler	: register( s1 );

// Linear wrap sampler ( used for UI ).
SamplerState s_ui_sampler		: register( s2 );

#endif // #ifndef __shader_samplers_h_included_