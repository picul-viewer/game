#ifndef GUARD_RESOURCE_SYSTEM_FILE_PTR_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_FILE_PTR_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"

namespace resource_system {

class file_ptr
{
public:
	file_ptr( ) = default;

	file_ptr( file_ptr const& in_other );
	file_ptr& operator=( file_ptr const& in_other );

	file_ptr( file_ptr&& in_other );
	file_ptr& operator=( file_ptr&& in_other );

	~file_ptr( );

	pointer data( ) const;
	u32 size( ) const;

private:
	friend class queried_resources;
	file_ptr( query_result const in_query_result );

private:
	friend class resource_system;
	static query_result load( pcstr const in_file_path, u32 const in_initial_ref_count );

private:
	pointer m_data = nullptr;

};

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_FILE_PTR_H_INCLUDED