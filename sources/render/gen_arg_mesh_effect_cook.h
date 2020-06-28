#ifndef GUARD_RENDER_GEN_ARG_MESH_EFFECT_COOK_H_INCLUDED
#define GUARD_RENDER_GEN_ARG_MESH_EFFECT_COOK_H_INCLUDED

#include <types.h>
#include <resource_system/default_resource_cook.h>
#include "pipeline_state.h"

namespace render {

class gen_arg_mesh_effect_cook : public default_resource_cook<pipeline_state, gen_arg_mesh_effect_cook>
{
public:
	enum type
	{
		type_mesh = 0,
		type_sun_shadowmap
	};

public:
	static gen_arg_mesh_effect_cook* create( pipeline_state* const in_ptr, type const in_type );
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
	type m_type;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GENE_ARG_MESH_EFFECT_COOK_H_INCLUDED