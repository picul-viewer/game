#ifndef GUARD_RENDER_SHADE_EFFECT_COOK_H_INCLUDED
#define GUARD_RENDER_SHADE_EFFECT_COOK_H_INCLUDED

#include <types.h>
#include <resource_system/default_resource_cook.h>
#include "pipeline_state.h"

namespace render {

class shade_effect_cook : public default_resource_cook<pipeline_state, shade_effect_cook>
{
public:
	shade_effect_cook( pipeline_state* const in_ptr );

public:
	enum : u32 {
		create_thread_index = engine_thread_main
	};

	void create_resource( );

private:
	void on_shaders_ready( queried_resources& in_resources );

private:
	pipeline_state* m_result;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_SHADE_EFFECT_COOK_H_INCLUDED