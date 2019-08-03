#include "config.h"
#include <macros.h>
#include <lib/algorithms.h>
#include <lib/hash.h>
#include <lib/strings.h>
#include <math/math_common.h>
#include "parse.h"

namespace utils {

struct node_info
{
	config_node_type m_type;
	union
	{
		bool m_bool;
		s64 m_int;
		double m_float;
		struct
		{
			u32 begin;
			u32 size;
		} m_str;
		struct
		{
			u32 first;
			u32 count;
		} m_vector_list;
		struct
		{
			u32 map;
			u32 size;
		} m_object;
	};
};

bool has_equal_types( pointer const arena, u32 const left, u32 const right )
{
	node_info const& l = *(node_info*)( arena + left );
	node_info const& r = *(node_info*)( arena + right );

	switch ( l.m_type )
	{
		case config_node_type_bool:
		case config_node_type_int:
		case config_node_type_float:
		case config_node_type_string:
		case config_node_type_object:
			return l.m_type == r.m_type;

		case config_node_type_vector:
		{
			if ( l.m_vector_list.count != r.m_vector_list.count )
				return false;

			u32* const l_table = arena + l.m_vector_list.first;
			u32* const r_table = arena + r.m_vector_list.first;

			for ( u32 i = 0; i < l.m_vector_list.count; ++i )
			{
				if ( !has_equal_types( arena, l_table[i], r_table[i] ) )
					return false;
			}

			return true;
		}

		case config_node_type_list:
		{
			if ( l.m_vector_list.count != r.m_vector_list.count )
				return false;

			u32* const l_table = arena + l.m_vector_list.first;
			u32* const r_table = arena + r.m_vector_list.first;

			return has_equal_types( arena, *l_table, *r_table );
		}

		default:
			UNREACHABLE_CODE
			return false;
	}
}

struct object_property_info
{
	u32 m_value;

#pragma warning (push)
#pragma warning (disable:4200)
	char m_key[0];
#pragma warning (pop)
};

config_node::config_node( config const& config, node_info const& info ) :
	m_config( config ),
	m_info( info )
{ }

bool config_node::valid( ) const
{
	return &m_info != &invalid( );
}

config_node_type config_node::type( ) const
{
	return m_info.m_type;
}

bool config_node::as_bool( ) const		
{
	ASSERT_CMP( type( ), ==, config_node_type_bool );
	return m_info.m_bool;
}

s64 config_node::as_int( ) const		
{
	ASSERT_CMP( type( ), ==, config_node_type_int );
	return m_info.m_int;
}

double config_node::as_float( ) const	
{
	ASSERT_CMP( type( ), ==, config_node_type_float );
	return m_info.m_float;
}

pcstr config_node::as_string( ) const	
{
	ASSERT_CMP( type( ), ==, config_node_type_string );
	return (pcstr)( m_config.arena( ) + m_info.m_str.begin );
}

config_node config_node::operator( )( uptr const index ) const
{
	ASSERT_CMP( type( ), ==, config_node_type_vector );
	ASSERT_CMP( index, <, m_info.m_vector_list.count );

	u32* const offsets = m_config.arena( ) + m_info.m_vector_list.first;
	u32 const offset = offsets[index];

	node_info& child_node = *(node_info*)( m_config.arena( ) + offset );
	return config_node( m_config, child_node );
}

config_node config_node::operator[]( uptr const index ) const
{
	ASSERT_CMP( type( ), ==, config_node_type_list );
	ASSERT_CMP( index, <, m_info.m_vector_list.count );

	u32* const offsets = m_config.arena( ) + m_info.m_vector_list.first;
	u32 const offset = offsets[index];

	node_info& child_node = *(node_info*)( m_config.arena( ) + offset );
	return config_node( m_config, child_node );
}

config_node config_node::operator[]( pcstr const key ) const
{
	ASSERT_CMP( type( ), ==, config_node_type_object );

	uptr const length = strings::length( key );
	u32 const key_hash = lib::hash32( key, length );

	u32 const table_size = m_info.m_object.size;
	u8 const* const table = m_config.arena( ) + m_info.m_object.map;
	u32 const* const values = m_config.arena( ) + m_info.m_object.map + table_size * sizeof(u8);

	u32 index = key_hash % table_size;

	for ( u32 i = 1; i < 256; ++i, index = ( index + 1 ) % table_size )
	{
		if ( table[index] != i )
			continue;

		object_property_info const& prop = *(object_property_info*)( m_config.arena( ) + values[index] );

		if ( !strings::equal( key, prop.m_key ) )
			continue;

		node_info& prop_value = *(node_info*)( m_config.arena( ) + prop.m_value );
		return config_node( m_config, prop_value );
	}

	return config_node( m_config, invalid( ) );
}

node_info& config_node::invalid( ) const
{
	// This can be invalid, because everything should be aligned by sizeof(u64).
	return *(node_info*)( m_config.arena( ) + 1 );
}


config::config( pvoid const memory, uptr const memory_size )
{
	create( memory, memory_size );
}

void config::create( pvoid const memory, uptr const memory_size )
{
	m_allocator.create( memory, memory_size );
}

config_node config::root( ) const
{
	return config_node( *this, *(node_info*)arena( ) );
}

pointer config::arena( ) const
{
	return m_allocator.data( );
}

pcstr config::source( ) const
{
	return m_source;
}

config_error_info const& config::error_info( ) const
{
	return m_error;
}

bool config::parse( pcstr const s )
{
	m_allocator.clear( );
	m_source = s;
	m_error.message = nullptr;

	node_info* const root = m_allocator.allocate( sizeof(node_info) );

	pcstr str = s;

	u32 const max_root_elements = 1024;

	u32 root_props[max_root_elements];
	u32 root_prop_hashes[max_root_elements];

	u32 prop_count = 0;

	skip_spaces( str );

	if ( *str != '\0' )
	{
		do
		{
			pcstr const key_start = str;
			
			if ( !is_identifier_symbol( *str ) )
			{
				error( str, "identifier expected" );
				return false;
			}

			do ++str;
			while ( is_identifier_symbol( *str ) );

			uptr const key_size = str - key_start;

			skip_spaces( str );

			if ( *str != '=' )
			{
				error( str, "'=' expected" );
				return false;
			}

			++str;
			skip_spaces( str );

			u32 value;
			if ( !parse_entity( str, value ) )
				return false;

			if ( !is_space( *str ) && ( *str != '\0' ) )
			{
				error( str, "unexpected symbol" );
				return false;
			}

			skip_spaces( str );

			uptr const memory_size = math::align_up( sizeof(node_info) + key_size + 1, sizeof(u64) );
			object_property_info* const prop = m_allocator.allocate( memory_size );
			prop->m_value = value;
			strings::copy_n( prop->m_key, key_start, key_size );
			prop->m_key[key_size] = '\0';

			if ( prop_count >= max_root_elements )
			{
				error( str, "too many root fields" );
				return false;
			}

			root_props[prop_count] = (u32)m_allocator.index_of( prop );
			root_prop_hashes[prop_count] = lib::hash32( prop->m_key, key_size );
			++prop_count;
		}
		while ( *str != '\0' );
	}

	u32 table;
	u32 table_size;
	if ( !build_hash_table( str, root_props, root_prop_hashes, prop_count, table, table_size ) )
		return false;

	root->m_type = config_node_type_object;
	root->m_object.map = table;
	root->m_object.size = table_size;

	return true;
}

bool config::parse_entity( pcstr& str, u32& value )
{
	switch ( *str )
	{
		case '(':
			return parse_vector( ++str, value );
		case '[':
			return parse_list( ++str, value );
		case '{':
			return parse_object( ++str, value );
		default:
			return parse_scalar( str, value );
	}
}

bool config::parse_scalar( pcstr& str, u32& value )
{
	switch ( *str )
	{
		case 't':
		case 'f':
		{
			node_info* const info = m_allocator.allocate( sizeof(node_info) );

			info->m_type = config_node_type_bool;
			if ( !parse_value( str, info->m_bool ) )
				break;
			
			value = (u32)m_allocator.index_of( info );
			return true;
		}

		case '"':
		{
			node_info* const info = m_allocator.allocate( sizeof(node_info) );

			str256 s;
			if ( !parse_value( str, s ) )
				break;

			uptr const length = s.length( );
			pstr const str_value = m_allocator.allocate( math::align_up( length + 1, sizeof(u64) ) );
			strings::copy_n( str_value, s.c_str( ), length + 1 );

			info->m_type = config_node_type_string;
			info->m_str.begin = (u32)m_allocator.index_of( str_value );
			info->m_str.size = (u32)length;
			
			value = (u32)m_allocator.index_of( info );
			return true;
		}

		case '+':
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			node_info* const info = m_allocator.allocate( sizeof(node_info) );

			pcstr original_str = str;
			if ( !parse_value( str, info->m_int ) )
				break;

			if ( *str != '.' )
				info->m_type = config_node_type_int;
			else
			{
				str = original_str;
				if ( !parse_value( str, info->m_float ) )
					break;

				info->m_type = config_node_type_float;
			}
			
			value = (u32)m_allocator.index_of( info );
			return true;
		}

		case '.':
		{
			node_info* const info = m_allocator.allocate( sizeof(node_info) );

			info->m_type = config_node_type_float;
			if ( !parse_value( str, info->m_float ) )
				break;
			
			value = (u32)m_allocator.index_of( info );
			return true;
		}
	}

	error( str, "wrong value expression" );
	return false;
}

bool config::parse_vector( pcstr& str, u32& value )
{
	u32 const max_vector_elements = 1024;

	u32 vector_elements[max_vector_elements];
	u32 i = 0;

	skip_spaces( str );

	if ( *str != ')' )
	{
		do
		{
			if ( i >= max_vector_elements )
			{
				error( str, "too many vector elements" );
				return false;
			}

			skip_spaces( str );
			if ( !parse_entity( str, vector_elements[i] ) )
				return false;

			++i;
			skip_spaces( str );
		}
		while ( *str++ == ',' );

		if ( *( str - 1 ) != ')' )
		{
			error( str - 1, "')' expected" );
			return false;
		}
	}

	node_info* const info = m_allocator.allocate( sizeof(node_info) );
	u32* const first = m_allocator.allocate( math::align_up( sizeof(u32) * i, sizeof(u64) ) );
	memory::copy( first, vector_elements, sizeof(u32) * i );

	info->m_type = config_node_type_vector;
	info->m_vector_list.first = (u32)m_allocator.index_of( first );
	info->m_vector_list.count = i;

	value = (u32)m_allocator.index_of( info );
	return true;
}

bool config::parse_list( pcstr& str, u32& value )
{
	u32 const max_list_elements = 1024;

	u32 list_elements[max_list_elements];
	u32 i = 0;

	skip_spaces( str );

	if ( *str != ']' )
	{
		if ( !parse_entity( str, list_elements[i] ) )
			return false;

		++i;
		skip_spaces( str );

		while ( *str++ == ',' )
		{
			if ( i >= max_list_elements )
			{
				error( str, "too many list elements" );
				return false;
			}

			skip_spaces( str );

			if ( !parse_entity( str, list_elements[i] ) )
				return false;

			if ( !has_equal_types( arena( ), list_elements[i], list_elements[0] ) )
			{
				error( str, "list elements have incompatible types" );
				return false;
			}

			++i;
			skip_spaces( str );
		}

		if ( *( str - 1 ) != ']' )
		{
			error( str - 1, "']' expected" );
			return false;
		}
	}

	node_info* const info = m_allocator.allocate( sizeof(node_info) );

	u32* const first = m_allocator.allocate( math::align_up( sizeof(u32) * i, sizeof(u64) ) );
	memory::copy( first, list_elements, sizeof(u32) * i );

	info->m_type = config_node_type_list;
	info->m_vector_list.first = (u32)m_allocator.index_of( first );
	info->m_vector_list.count = i;

	value = (u32)m_allocator.index_of( info );
	return true;
}

bool config::parse_object( pcstr& str, u32& value )
{
	u32 const max_object_elements = 1024;

	u32 object_props[max_object_elements];
	u32 object_prop_hashes[max_object_elements];

	u32 prop_count = 0;

	skip_spaces( str );

	if ( *str != '}' )
	{
		do
		{
			skip_spaces( str );

			pcstr const key_start = str;

			if ( !is_identifier_symbol( *str ) )
			{
				error( str, "identifier expected" );
				return false;
			}

			do ++str;
			while ( is_identifier_symbol( *str ) );

			uptr const key_size = str - key_start;

			skip_spaces( str );

			if ( *str != '=' )
			{
				error( str, "'=' expected" );
				return false;
			}

			++str;
			skip_spaces( str );

			u32 value;
			if ( !parse_entity( str, value ) )
				return false;

			skip_spaces( str );

			uptr const memory_size = math::align_up( sizeof(node_info) + key_size + 1, sizeof(u64) );
			object_property_info* const prop = m_allocator.allocate( memory_size );
			prop->m_value = value;
			strings::copy_n( prop->m_key, key_start, key_size );
			prop->m_key[key_size] = '\0';

			if ( prop_count >= max_object_elements )
			{
				error( str, "too many root fields" );
				return false;
			}

			object_props[prop_count] = (u32)m_allocator.index_of( prop );
			object_prop_hashes[prop_count] = lib::hash32( prop->m_key, key_size );
			++prop_count;
		}
		while ( *str++ == ',' );

		if ( *( str - 1 ) != '}' )
		{
			error( str - 1, "'}' expected" );
			return false;
		}
	}

	u32 table;
	u32 table_size;
	if ( !build_hash_table( str - 1, object_props, object_prop_hashes, prop_count, table, table_size ) )
		return false;

	node_info* const info = m_allocator.allocate( sizeof(node_info) );
	info->m_type = config_node_type_object;
	info->m_object.map = table;
	info->m_object.size = table_size;

	value = (u32)m_allocator.index_of( info );
	return true;
}

bool config::build_hash_table( pcstr const str, u32 const* const object_props, u32 const* const object_prop_hashes, u32 const prop_count, u32& value, u32& table_size )
{
	table_size = math::align_up( prop_count, 8 );

	pointer const table = m_allocator.allocate( table_size * ( sizeof(u8) + sizeof(u32) ) );
	u8* const keys = table;
	u32* const values = table + sizeof(u8) * table_size;

	value = (u32)m_allocator.index_of( table );

	memory::zero( keys, sizeof(u8) * table_size );

	for ( u32 i = 0; i < prop_count; ++i )
	{
		u32 index = object_prop_hashes[i] % table_size;
		u8 j;
		u32 current_key;

		for ( j = 1; ; ++j )
		{
			current_key = keys[index];

			if ( j > current_key )
				break;

			index = ( index + 1 ) % table_size;

			if ( j == 0xFF )
			{
				error( str, "unable to build a hash table" );
				return false;
			}
		}

		u8 key_to_insert = current_key;
		u32 value_to_insert = values[index];

		keys[index] = j;
		values[index] = object_props[i];

		while ( key_to_insert != 0 )
		{
			index = ( index + 1 ) % table_size;

			current_key = keys[index];

			if ( ( key_to_insert & 0xFF ) == 0xFF )
			{
				error( str, "unable to build a hash table" );
				return false;
			}

			++key_to_insert;

			if ( current_key < key_to_insert )
			{
				lib::swap( keys[index], key_to_insert );
				lib::swap( values[index], value_to_insert );
			}
		}
	}

	return true;
}

void config::error( pcstr const str, pcstr const message )
{
	m_error.position = str - m_source;
	m_error.message = message;

	m_error.line_index = 1;
	m_error.line_position = 1;

	for ( pcstr i = m_source; i < str; ++i )
	{
		if ( *i == '\n' )
		{
			++m_error.line_index;
			m_error.line_position = 1;
		}
		else
			++m_error.line_position;
	}
}

void config::log_error( pcstr const config_name ) const
{
	if ( m_error.message == nullptr )
		return;

	if ( config_name )
		LOG( "Error found when parsing config '%s':\n", config_name );
	else
		LOG( "Error found when parsing config:\n" );

	LOG( "\tline %d, column %d: %s\n", m_error.line_index, m_error.line_position, m_error.message );
}

} // namespace utils