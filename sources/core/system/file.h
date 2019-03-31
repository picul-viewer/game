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
	file( );
	file( pcstr path, open_mode mode );
	~file( );

	void open( pcstr path, open_mode mode );
	void close( );

	uptr size( ) const;

	void seek( ptr position, seek_mode mode = seek_from_begin );

	void read( pvoid buffer, uptr size );
	void write( pvoid buffer, uptr size );

	bool is_valid( ) const;

protected:
	pvoid m_handle;

};

} // namespace sys

#endif // #ifndef GUARD_CORE_FILE_H_INCLUDED