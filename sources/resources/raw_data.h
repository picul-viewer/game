#ifndef GUARD_RESOURCES_RAW_DATA_H_INCLUDED
#define GUARD_RESOURCES_RAW_DATA_H_INCLUDED

#include <types.h>
#include <resource_system/default_resource.h>
#include <resource_system/default_resource_cook.h>
#include <utils/engine_threads.h>

namespace resources {

class raw_data;

class raw_data_cook : public default_resource_cook<raw_data, raw_data_cook>
{
public:
	raw_data_cook( pcstr const in_path );
	static uptr size( pcstr const in_path );

public:
	enum : u32 {
		create_thread_index = engine_thread_fs,
	};

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
	static raw_data* create( uptr const size );

	pointer data( ) const;
	uptr size( ) const;

private:
	uptr m_size;

#pragma warning (push)
#pragma warning (disable:4200)
	u8 m_data[0];
#pragma warning (pop)

};

} // namespace resource_system

using resources::raw_data;
using resources::raw_data_cook;

#endif // #ifndef GUARD_RESOURCES_RAW_DATA_H_INCLUDED