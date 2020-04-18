#include "macros.h"
#include <system/file.h>
#include <Windows.h>

namespace macros {

class file_writer
{
public:
	file_writer( )
	{
		m_file.create( "log.txt", sys::file::open_write );
	}

	~file_writer( )
	{
		m_file.destroy( );
	}

	void write( char const* const message )
	{
		m_file.write( message, strlen(message) );
	}

private:
	sys::file m_file;

} g_file_writer;

void debug_string( char const* message )
{
	g_file_writer.write( message );
	OutputDebugString( message );
}

void pause( )
{
	_mm_pause( );
}

void sleep( u32 const milliseconds )
{
	Sleep( milliseconds );
}

} // namespace macros