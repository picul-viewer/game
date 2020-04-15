#include "pipeline_state.h"
#include <lib/allocator.h>

namespace render {

pipeline_state* pipeline_state::create_resource( pipeline_state* const in_ptr, dx_root_signature const& in_rs, dx_pipeline_state const& in_ps )
{
	ASSERT( in_ptr );
	in_ptr->m_ps = in_ps;
	in_ptr->m_rs = in_rs;
	return in_ptr;
}

void pipeline_state::destroy_resource( pipeline_state* const in_resource )
{
	in_resource->m_ps.destroy( );
	in_resource->m_rs.destroy( );
}

} // namespace render