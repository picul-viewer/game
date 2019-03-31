#ifndef GUARD_RESOURCE_POOL_H_INCLUDED
#define GUARD_RESOURCE_POOL_H_INCLUDED

#include <types.h>
#include <intrin.h>
#include <lib/allocator.h>
#include <system/interlocked.h>
#include "resource_system_types.h"

namespace resource_system {

template<typename Resource, uptr MaxSize>
class resource_pool
{
public:
	void create( pointer const in_reserved_memory = nullptr );
	void destroy( );

	bool get( resource_instance_id const in_resource_instance_id, u32& out_index );

public:
	enum : uptr {
		table_size = sizeof(u8) * MaxSize,
		data_size = sizeof(u32) * MaxSize,

		memory_size = table_size + data_size
	};

private:
	u8* m_table;
	u32* m_data;

};

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
class resource_pool1
{
public:
	void create( pointer const in_reserved_memory = nullptr );
	void destroy( );

	bool get( resource_instance_id const in_resource_instance_id, u32& out_index );

public:
	enum : uptr {
		table_record_size = sizeof(u32) * 2,

		table_size = table_record_size * MaxSize,
		data_size = sizeof(Resource) * MaxSize,

		memory_size = table_size + data_size
	};

private:
	pointer		m_table;
	Resource*	m_data;

	u32			m_push_index;
	u32			m_last_index;

};

template<typename Resource, uptr TableSize, uptr MaxSize, uptr PageSize>
class resource_pool2
{
public:
	void create( pointer const in_reserved_memory = nullptr );
	void destroy( );

	bool get( resource_instance_id const in_resource_instance_id, u32& out_index );

	u32 add_ref( u32 const in_resource_id );
	u32 release( u32 const in_resource_id );

public:
	enum : uptr {
		table_size = ( sizeof(u32) * MaxSize + PageSize - 1 ) / PageSize * PageSize,
		data_size = ( sizeof(Resource) * MaxSize + PageSize - 1 ) / PageSize * PageSize,

		memory_size = table_size + data_size
	};

private:
	u32*		m_table;
	Resource*	m_data;

	u32			m_push_index;
	u32			m_last_index;

};

} // namespace resource_system

#include "resource_pool_inline.h"

#endif // #ifndef GUARD_RESOURCE_POOL_H_INCLUDED