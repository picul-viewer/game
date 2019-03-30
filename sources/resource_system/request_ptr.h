#ifndef __resource_system_request_ptr_h_included_
#define __resource_system_request_ptr_h_included_

#include <types.h>
#include "resource_system_types.h"

namespace resource_system {

class request_ptr
{
public:
	request_ptr( ) = default;

	request_ptr( request_ptr const& in_other );
	request_ptr& operator=( request_ptr const& in_other );

	request_ptr( request_ptr&& in_other );
	request_ptr& operator=( request_ptr&& in_other );

	~request_ptr( );

private:
	friend class request_builder;
	request_ptr( pointer const in_data );
	
private:
	friend class queried_resources;
	request_ptr( query_result const in_query_result );

private:
	friend class resource_system;
	static query_result load( pcstr const in_file_path, u32 const in_initial_ref_count );

private:
	friend class resource_system;
	void submit( query_callback_id const in_callback_id );

	static void release_by_pointer( pointer const in_request );

private:
	pointer m_data;

};

} // namespace resource_system

#endif // #ifndef __resource_system_request_ptr_h_included_