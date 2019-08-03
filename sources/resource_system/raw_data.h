#ifndef GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED

#include <types.h>
#include <system/interlocked.h>
#include <utils/engine_threads.h>
#include "default_resource.h"
#include "default_resource_cook.h"
#include "resource_handle.h"

namespace resource_system {

DECLARE_DEFAULT_RESOURCE_HANDLE( raw_data, raw_data* )

class raw_data_cook : public default_resource_cook<raw_data, raw_data_cook>
{
public:
	enum : u32 {
		create_thread_index = engine_thread_fs,
	};

public:
	static raw_data_cook* create( pcstr const in_path );
	static void destroy( pointer const in_cook );

	void create_resource( );

private:
#pragma warning (push)
#pragma warning (disable:4200)
	char m_path[0];
#pragma warning (pop)

};

class raw_data : public default_resource<raw_data>
{
public:
	friend class raw_data_cook;

	typedef raw_data_handle handle_type;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	// Override "delete_resource" to delete resource in place and avoid resource system overhead.
	void delete_resource( );

public:
	static raw_data* from_handle( raw_data_handle const& in_handle );
	static raw_data_handle to_handle( raw_data* const in_resource );
	static raw_data* to_handle_value( raw_data* const in_resource );

	pointer data( ) const;
	uptr size( ) const;

private:
	raw_data( ) = default;

private:
	uptr m_size;

#pragma warning (push)
#pragma warning (disable:4200)
	u8 m_data[0];
#pragma warning (pop)

};

} // namespace resource_system

using resource_system::raw_data;
using resource_system::raw_data_cook;
using resource_system::raw_data_handle;
using raw_data_ptr = raw_data::ptr;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED