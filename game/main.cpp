#include <types.h>
#include <engine/world.h>

int __stdcall WinMain( HINSTANCE, HINSTANCE, char*, int )
{
	engine::g_world.run( );
	return 0;
}