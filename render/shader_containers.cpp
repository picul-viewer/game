#include "shader_containers.h"

#ifdef DEBUG
#	define VERTEX_PATH( name )		( "resources/shaders_debug/vertex/" name )
#	define PIXEL_PATH( name )		( "resources/shaders_debug/pixel/" name )
#	define GEOMETRY_PATH( name )	( "resources/shaders_debug/geometry/" name )
#	define HULL_PATH( name )		( "resources/shaders_debug/hull/" name )
#	define DOMAIN_PATH( name )		( "resources/shaders_debug/domain/" name )
#	define COMPUTE_PATH( name )		( "resources/shaders_debug/compute/" name )
#else
#	define VERTEX_PATH( name )		( "resources/shaders/vertex/" name )
#	define PIXEL_PATH( name )		( "resources/shaders/pixel/" name )
#	define GEOMETRY_PATH( name )	( "resources/shaders/geometry/" name )
#	define HULL_PATH( name )		( "resources/shaders/hull/" name )
#	define DOMAIN_PATH( name )		( "resources/shaders/domain/" name )
#	define COMPUTE_PATH( name )		( "resources/shaders/compute/" name )
#endif // #ifdef DEBUG

namespace render {

namespace __render_shader_containers_detail {

#pragma warning( push )
#pragma warning( disable : 4065 )

pcstr vertex_shader_path_provider::get( vertex_shader_type type )
{
	switch ( type )
	{
	case vertex_shader_forward_default:
		return VERTEX_PATH( "forward_default_0000000000000000" );
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
		return PIXEL_PATH( "forward_default_0000000000000000" );
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