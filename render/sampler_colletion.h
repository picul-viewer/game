#ifndef __render_sampler_collection_h_included_
#define __render_sampler_collection_h_included_

#include "sampler.h"

namespace render {

namespace sampler_collection {

enum { samplers_count = 2 };

void create( );
void destroy( );

sampler const& get_texture_sampler( );
sampler const& get_point_sampler( );
	
void bind_vs( );
void bind_ps( );
void bind_gs( );
void bind_hs( );
void bind_ds( );
void bind_cs( );

} // namespace samplers

} // namespace render

#endif // #ifndef __render_sampler_collection_h_included_