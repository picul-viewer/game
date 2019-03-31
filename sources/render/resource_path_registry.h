#ifndef __render_resource_path_registry_h_inlcuded_
#define __render_resource_path_registry_h_inlcuded_

#include <lib/fixed_string.h>
#include <lib/hash_map.h>

namespace render {

namespace __render_resource_path_registry {

struct resource_path_registry_key_hash_pred
{
	typedef lib::hash32::value_type type;

	lib::hash32::value_type operator( )( char const& value );
};

struct resource_path_registry_key_equal_pred
{
	bool operator( )( char const& l, char const& r );
};

struct resource_path_registry_key_value_store
{
	pointer m_next;
	pointer m_prev;
	u32 m_value;
	// This char actually represents unexpected-length string.
	char m_key;

	resource_path_registry_key_value_store( ) = default;
	resource_path_registry_key_value_store( char const& key, u32 value );

	char& key( );
	u32& value( );
	pointer& next( );
	pointer& prev( );
	
	void set_key( char const& key );
	void set_value( u32 value );
	void set_next( pointer next );
	void set_prev( pointer prev );
	
	static uptr size( char const& k, u32 );
};

struct resource_path_registry_key_value_store_index
{
	typedef pointer value_type;

	static const pointer null_value;

	static pointer get( std_allocator&, value_type index );
	static value_type index_of( std_allocator&, pointer kv );
};

} // namespace __render_resource_path_registry

typedef lib::hash_map_template<
	char const&, u32,
	__render_resource_path_registry::resource_path_registry_key_hash_pred,
	__render_resource_path_registry::resource_path_registry_key_equal_pred,
	__render_resource_path_registry::resource_path_registry_key_value_store,
	__render_resource_path_registry::resource_path_registry_key_value_store_index,
	std_allocator> resource_path_registry;

} // namespace render

#endif // #ifndef __render_resource_path_registry_h_inlcuded_