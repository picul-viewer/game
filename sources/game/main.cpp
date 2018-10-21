#include <types.h>
#include <engine/world.h>
#include <Windows.h>

int __stdcall WinMain( HINSTANCE, HINSTANCE, char*, int )
{
	engine::g_world.run( math::u32x2( 800, 600 ) );
	return 0;
}