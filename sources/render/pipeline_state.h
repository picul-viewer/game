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
	static void destroy_resource( pipeline_state* const in_resource );

public:
	inline dx_root_signature const& rs( ) const { return m_rs; }
	inline dx_pipeline_state const& ps( ) const { return m_ps; }

private:
	dx_root_signature m_rs;
	dx_pipeline_state m_ps;

};

template<typename CookType>
class simple_pipeline_state_cook : public default_resource_cook<pipeline_state, CookType>
{
public:
	static CookType* create( CookType* const in_ptr, pipeline_state* const in_resource_ptr )
	{
		in_ptr->init( );
		in_ptr->m_result = in_resource_ptr;
		return in_ptr;
	}

	static inline void destroy( pointer const in_cook ) { }

protected:
	inline pipeline_state* result_ptr( ) const { return (pipeline_state*)m_result; }

private:
	pipeline_state* m_result;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_PIPELINE_STATE_H_INCLUDED