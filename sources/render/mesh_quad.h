#include <types.h>
#include <math/vector.h>

namespace render {

namespace quad {

static const math::float3 vertices[] = {
	math::float3( -1.0f, -1.0f, 0.0f ),
	math::float3(  1.0f, -1.0f, 0.0f ),
	math::float3(  1.0f,  1.0f, 0.0f ),
	math::float3( -1.0f,  1.0f, 0.0f )
};

static const u16 indices[] = {
	0u, 1u, 2u,
    0u, 2u, 3u
};

static const u32 index_count = 6;

} // namespace quad

} // namespace render