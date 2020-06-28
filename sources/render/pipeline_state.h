#ifndef GUARD_RENDER_PIPELINE_STATE_H_INCLUDED
#define GUARD_RENDER_PIPELINE_STATE_H_INCLUDED

#include <types.h>
#include <resource_system/default_resource.h>
#include <resource_system/default_resource_cook.h>
#include <utils/engine_threads.h>
#include "dx_pipeline_state.h"

namespace render {

class pipeline_state : public default_resource<pipeline_state>
{
public:
	friend class pipeline_state;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	static pipeline_state* create_resource( pipeline_state* const in_ptr, dx_root_signature const& in_rs, dx_pipeline_state const& in_ps );

	void destroy( );

public:
	inline dx_root_signature const& rs( ) const { return m_rs; }
	inline dx_pipeline_state const& ps( ) const { return m_ps; }

private:
	dx_root_signature m_rs;
	dx_pipeline_state m_ps;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_PIPELINE_STATE_H_INCLUDED