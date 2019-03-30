#ifndef __resource_system_api_h_included_
#define __resource_system_api_h_included_

#include <types.h>
#include "request_ptr.h"
#include "resource_system_types.h"

namespace resource_system {

void create( u32 const in_worker_thread_count );
void destroy( );

void process_request( request_ptr& in_request, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size );
void process_request_from_file( pcstr const in_file_path, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size );
void query_file( pcstr const in_file_path, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size );
void query_request( pcstr const in_file_path, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size );

} // namespace resource_system

#endif // #ifndef __resource_system_api_h_included_