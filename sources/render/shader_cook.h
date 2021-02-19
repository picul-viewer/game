#ifndef GUARD_RENDER_SHADER_COOK_H_INCLUDED
#define GUARD_RENDER_SHADER_COOK_H_INCLUDED

#include <types.h>
#include <lib/fixed_string.h>
#include <resource_system/queried_resources.h>
#include <resource_system/default_resource_cook.h>
#include <resources/raw_data.h>
#include <system/path.h>
#include <utils/engine_threads.h>
#include "dx_pipeline_state.h"

namespace render {

struct shader_define
{
	pcstr name;
	fixed_string<56> value;
};

class shader_cook : public default_resource_cook<raw_data, shader_cook>
{
public:
	shader_cook(
		shader_type const in_type,
		pcstr const in_name,
		u32 const in_defines_count = 0,
		shader_define const* const in_defines = nullptr
	);
	static uptr size(
		shader_type const in_type,
		pcstr const in_name,
		u32 const in_defines_count = 0,
		shader_define const* const in_defines = nullptr
	);

public:
	enum : u32 {
		create_thread_index = engine_thread_main
	};

	void create_resource( );

private:
	void compile_shader( queried_resources& in_resources );

private:
	sys::path m_source_path;
	pcstr m_name;
	shader_type m_type;
	u32 m_defines_count;

#pragma warning (push)
#pragma warning (disable:4200)
	shader_define m_defines[0];
#pragma warning (pop)
};

} // namespace render

#endif // #ifndef GUARD_RENDER_SHADER_COOK_H_INCLUDED