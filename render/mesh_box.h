#include <types.h>
#include <math/vector.h>

namespace render {

namespace box {

const math::float3 vertices[] = {
	math::float3( -1.0f, -1.0f, -1.0f ),
	math::float3(  1.0f, -1.0f, -1.0f ),
	math::float3(  1.0f,  1.0f, -1.0f ),
	math::float3( -1.0f,  1.0f, -1.0f ),
	math::float3( -1.0f, -1.0f,  1.0f ),
	math::float3(  1.0f, -1.0f,  1.0f ),
	math::float3(  1.0f,  1.0f,  1.0f ),
	math::float3( -1.0f,  1.0f,  1.0f )
};

const u16 indices[] = {
	0u, 1u, 2u,
    0u, 2u, 3u,
    4u, 5u, 6u,
    4u, 6u, 7u,
    3u, 2u, 5u,
    3u, 5u, 4u,
    2u, 1u, 6u,
    2u, 6u, 5u,
    1u, 7u, 6u,
    1u, 0u, 7u,
    0u, 3u, 4u,
    0u, 4u, 7u
};

const u32 index_count = 36;

} // namespace box

} // namespace render