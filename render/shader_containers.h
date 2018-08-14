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


namespace __render_shader_containers_detail
{
	struct vertex_shader_path_provider
	{
		static pcstr get( vertex_shader_type type );
	};

	struct pixel_shader_path_provider
	{
		static pcstr get( pixel_shader_type type );
	};

	struct geometry_shader_path_provider
	{
		static pcstr get( geometry_shader_type type );
	};

	struct hull_shader_path_provider
	{
		static pcstr get( hull_shader_type type );
	};

	struct domain_shader_path_provider
	{
		static pcstr get( domain_shader_type type );
	};

	struct compute_shader_path_provider
	{
		static pcstr get( compute_shader_type type );
	};
}

typedef shader_and_input_layout_container<
	vertex_shader,
	vertex_shader_type,
	vertex_shader_type_count,
	__render_shader_containers_detail::vertex_shader_path_provider> vertex_shader_container;

typedef shader_container<
	pixel_shader,
	pixel_shader_type,
	pixel_shader_type_count,
	__render_shader_containers_detail::pixel_shader_path_provider> pixel_shader_container;

typedef shader_container<
	geometry_shader,
	geometry_shader_type,
	geometry_shader_type_count,
	__render_shader_containers_detail::geometry_shader_path_provider> geometry_shader_container;

typedef shader_container<
	hull_shader,
	hull_shader_type,
	hull_shader_type_count,
	__render_shader_containers_detail::hull_shader_path_provider> hull_shader_container;

typedef shader_container<
	domain_shader,
	domain_shader_type,
	domain_shader_type_count,
	__render_shader_containers_detail::domain_shader_path_provider> domain_shader_container;

typedef shader_container<
	compute_shader,
	compute_shader_type,
	compute_shader_type_count,
	__render_shader_containers_detail::compute_shader_path_provider> compute_shader_container;

} // namespace render

#endif // #ifndef __render_shader_containers_h_included_