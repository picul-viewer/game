#ifndef __core_file_h_included_
#define __core_file_h_included_

#include <types.h>

namespace sys {

class file
{
public:
	enum open_mode
	{
		open_read,
		open_write_create,
		open_write_append
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

	void close( );

	uptr size( ) const;

	void seek( ptr position, seek_mode mode = seek_from_begin );

	uptr read( pvoid buffer, uptr size );
	uptr write( pvoid buffer, uptr size );

	bool is_valid( ) const;

protected:
	int m_handle;
	uptr m_size;

};

} // namespace sys

#endif // #ifndef __core_file_h_included_