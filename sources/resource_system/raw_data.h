#ifndef GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED

#include <types.h>
#include <system/interlocked.h>
#include <utils/engine_threads.h>
#include "default_resource.h"
#include "default_resource_cook.h"
#include "resource_handle.h"

namespace resource_system {

class raw_data;

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

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	// Override "destroy" to destroy resource in place and avoid resource system overhead.
	void destroy( );

public:
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

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED