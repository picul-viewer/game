#ifndef GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED

#include <types.h>
#include "resource_cook.h"
#include "resource_ptr.h"
#include <system/interlocked.h>
#include <utils/resource_system_threads.h>
#include <utils/resource_types.h>

namespace resource_system {

class raw_data
{
public:
	friend class raw_data_cook;
	typedef raw_data_cook cook_type;

public:
	raw_data( raw_data const& ) = delete;
	raw_data& operator=( raw_data const& ) = delete;
	raw_data( raw_data&& ) = delete;
	raw_data& operator=( raw_data&& ) = delete;

	~raw_data( ) = default;

	u32 add_ref( );
	u32 release( );

	pointer data( ) const;
	u32 size( ) const;

private:
	raw_data( ) = default;

private:
	mt_u32 m_ref_count;
	u32 m_size;

#pragma warning (push)
#pragma warning (disable:4200)
	u8 m_data[0];
#pragma warning (pop)

};

class raw_data_cook : public resource_cook
{
public:
	raw_data_cook( pcstr const in_path );

	void create_resource( );
	
	static void destroy_resource( raw_data* const in_resource );

	enum : u32 {
		cook_resource_type = resource_type_raw_data,

		create_resource_thread_index = resource_system_thread_fs
	};

private:
	pcstr m_path;

};

typedef resource_ptr<raw_data> raw_data_ptr;

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RAW_DATA_H_INCLUDED