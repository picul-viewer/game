#ifndef __render_shader_containers_h_included_
#define __render_shader_containers_h_included_

#include <types.h>

#include "shader.h"
#include "shader_container.h"
#include "shader_and_input_layout_container.h"

namespace render {

enum vertex_shader_type
{
	vertex_shader_forward_default,

	vertex_shader_type_count
};

enum pixel_shader_type
{
	pixel_shader_forward_default,

	pixel_shader_type_count
};

enum geometry_shader_type
{
	geometry_shader_type_count
};

enum hull_shader_type
{
	hull_shader_type_count
};

enum domain_shader_type
{
	domain_shader_type_count
};

enum compute_shader_type
{
	compute_shader_type_count
};

typedef shader_and_input_layout_container<
	vertex_shader,
	vertex_shader_type,
	vertex_shader_type_count> vertex_shader_container;

typedef shader_container<
	pixel_shader,
	pixel_shader_type,
	pixel_shader_type_count> pixel_shader_container;

typedef shader_container<
	geometry_shader,
	geometry_shader_type,
	geometry_shader_type_count> geometry_shader_container;

typedef shader_container<
	hull_shader,
	hull_shader_type,
	hull_shader_type_count> hull_shader_container;

typedef shader_container<
	domain_shader,
	domain_shader_type,
	domain_shader_type_count> domain_shader_container;

typedef shader_container<
	compute_shader,
	compute_shader_type,
	compute_shader_type_count> compute_shader_container;

} // namespace render

#endif // #ifndef __render_shader_containers_h_included_