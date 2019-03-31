#ifndef GUARD_CORE_FILE_ITERATOR_H_INCLUDED
#define GUARD_CORE_FILE_ITERATOR_H_INCLUDED

#include <types.h>

namespace sys {

class file_iterator
{
public:
	void create( pcstr mask );
	void destroy( );

	bool next( );
	bool is_valid( ) const;

	bool is_directory( ) const;
	pcstr get_file_name( ) const;
	uptr get_file_size( ) const;
	u64 get_creation_time( ) const;
	u64 get_modification_time( ) const;

protected:
	enum { find_data_size = 320 };

	pvoid m_handle;
	u8 m_find_data[find_data_size];

};

} // namespace sys

#endif // #ifndef GUARD_CORE_FILE_ITERATOR_H_INCLUDED