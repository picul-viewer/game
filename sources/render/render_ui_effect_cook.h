#ifndef GUARD_RENDER_RENDER_UI_EFFECT_COOK_H_INCLUDED
#define GUARD_RENDER_RENDER_UI_EFFECT_COOK_H_INCLUDED

#include <types.h>
#include <resource_system/default_resource_cook.h>
#include "pipeline_state.h"

namespace render {

class render_ui_effect_cook : public default_resource_cook<pipeline_state, render_ui_effect_cook>
{
public:
	static render_ui_effect_cook* create( pipeline_state* const in_ptr );
	static void destroy( pointer const in_cook );

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

#endif // #ifndef GUARD_RENDER_RENDER_UI_EFFECT_COOK_H_INCLUDED