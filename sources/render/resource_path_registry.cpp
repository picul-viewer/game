#include "resource_path_registry.h"

namespace render {

namespace __render_resource_path_registry {

lib::hash32::value_type resource_path_registry_key_hash_pred::operator( )( char const& value )
{
	return weak_const_string( &value ).hash( );
}


bool resource_path_registry_key_equal_pred::operator( )( char const& l, char const& r )
{
	return strings::equal( &l, &r );
}


resource_path_registry_key_value_store::resource_path_registry_key_value_store( char const& key, u32 value )
{
	set_key( key );
	m_value = value;
}

char& resource_path_registry_key_value_store::key( )
{
	return m_key;
}

u32& resource_path_registry_key_value_store::value( )
{
	return m_value;
}

pointer& resource_path_registry_key_value_store::next( )
{
	return m_next;
}

pointer& resource_path_registry_key_value_store::prev( )
{
	return m_prev;
}

void resource_path_registry_key_value_store::set_key( char const& key )
{
	strings::copy( &m_key, &key );
}

void resource_path_registry_key_value_store::set_value( u32 value )
{
	m_value = value;
}

void resource_path_registry_key_value_store::set_next( pointer next )
{
	m_next = next;
}

void resource_path_registry_key_value_store::set_prev( pointer prev )
{
	m_prev = prev;
}

uptr resource_path_registry_key_value_store::size( char const& k, u32 )
{
	return sizeof(resource_path_registry_key_value_store) + strings::length( &k ) + 1;
}


pointer resource_path_registry_key_value_store_index::get( std_allocator&, value_type index )
{
	return index;
}

resource_path_registry_key_value_store_index::value_type resource_path_registry_key_value_store_index::index_of( std_allocator&, pointer kv )
{
	return kv;
}

const pointer resource_path_registry_key_value_store_index::null_value = nullptr;

} // namespace __render_resource_path_registry

} // namespace render