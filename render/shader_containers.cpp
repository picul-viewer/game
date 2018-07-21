#include "shader_containers.h"

namespace render {

namespace __render_shader_containers_detail {

#pragma warning( push )
#pragma warning( disable : 4065 )

pcstr vertex_shader_path_provider::get( vertex_shader_type type )
{
	switch ( type )
	{
	case vertex_shader_forward_default:
		return "forward_default_0000000000000000.vs";
	default:
		UNREACHABLE_CODE
	}
	
	return nullptr;
}

pcstr pixel_shader_path_provider::get( pixel_shader_type type )
{
	switch ( type )
	{
	case pixel_shader_forward_default:
		return "forward_default_0000000000000000.ps";
	default:
		UNREACHABLE_CODE
	}

	return nullptr;
}

pcstr geometry_shader_path_provider::get( geometry_shader_type type )
{
	switch ( type )
	{
	default:
		UNREACHABLE_CODE
	}
	
	return nullptr;
}

pcstr hull_shader_path_provider::get( hull_shader_type type )
{
	switch ( type )
	{
	default:
		UNREACHABLE_CODE
	}
	
	return nullptr;
}

pcstr domain_shader_path_provider::get( domain_shader_type type )
{
	switch ( type )
	{
	default:
		UNREACHABLE_CODE
	}
	
	return nullptr;
}

pcstr compute_shader_path_provider::get( compute_shader_type type )
{
	switch ( type )
	{
	default:
		UNREACHABLE_CODE
	}
	
	return nullptr;
}

#pragma warning( pop ) 

} // namespace __render_shader_containers_detail

} // namespace render