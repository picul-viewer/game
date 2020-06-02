#ifndef GUARD_CORE_FILE_H_INCLUDED
#define GUARD_CORE_FILE_H_INCLUDED

#include <types.h>

namespace sys {

class file
{
public:
	enum open_mode
	{
		open_read,
		open_write
	};

	enum seek_mode
	{
		seek_from_begin = 0,
		seek_from_current,
		seek_from_end
	};

public:
	void create( pcstr path, open_mode mode );
	bool try_create( pcstr path, open_mode mode );
	void destroy( );

	uptr size( ) const;

	void seek( ptr position, seek_mode mode = seek_from_begin );

	void read( pvoid buffer, uptr size );
	void write( pcvoid buffer, uptr size );

	bool is_valid( ) const;

private:
	pvoid m_handle = (pvoid)-1;

};

} // namespace sys

#endif // #ifndef GUARD_CORE_FILE_H_INCLUDED