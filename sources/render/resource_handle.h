#ifndef __render_resource_handle_h_included_
#define __render_resource_handle_h_included_

#include "resource_ids.h"

namespace render {

template<typename Resource, typename IdType>
class resource_handle
{
public:
	enum { invalid = (IdType)-1 };

	resource_handle( ) : m_id( invalid ) { }

	resource_handle( IdType const in_value ) : m_id( in_value ) { }
	operator IdType( ) const { return m_id; }
	operator IdType&( ) { return m_id; }

	resource_handle( Resource* const in_resource ) : m_id( Resource::to_handle( in_resource ) ) { }
	operator Resource*( ) const { return Resource::from_handle( m_id ); }
	Resource* operator->( ) const { return (Resource*)*this; }

private:
	IdType m_id;

};

#define DEFINE_HANDLE( type ) typedef resource_handle<type, type##_id> type##_handle;

} // namespace render

#endif // #ifndef __render_resource_handle_h_included_