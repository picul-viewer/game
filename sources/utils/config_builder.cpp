#include "config_builder.h"
#include <macros.h>
#include <lib/strings.h>
#include <math/math_common.h>

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
			u32 offset;
			u32 length;
		} m_str;
		struct
		{
			u32 first;
			u32 count;
		} m_vector;
		struct
		{
			u32 first;
			u32 current;
		} m_list_object;
	};
};

struct list_element_node_info
{
	u32 m_value;
	u32 m_next;
};

struct object_element_node_info
{
	u32 m_value;
	u32 m_next;

#pragma warning (push)
#pragma warning (disable:4200)
	char m_key[0];
#pragma warning (pop)
};

config_builder_node::config_builder_node( config_builder& config_reader, node_info& info ) :
	m_config( config_reader ),
	m_info( &info )
{ }

node_info& config_builder_node::invalid( ) const
{
	// This can be invalid, because everything should be aligned by sizeof(u64).
	return *(node_info*)( m_config.allocator( ).data( ) + 1 );
}


config_builder_vector_node::config_builder_vector_node( config_builder& config, node_info& info ) :
	config_builder_node( config, info )
{ }

void config_builder_vector_node::set_element( u32 const index, config_builder_node const& node ) const
{
	ASSERT_CMP( index, <, m_info->m_vector.count );

	u32* const table = (u32*)( m_config.allocator( ).data( ) + m_info->m_vector.first );
	table[index] = (u32)m_config.allocator( ).index_of( node.m_info );
}


config_builder_list_node::config_builder_list_node( config_builder& config, node_info& info ) :
	config_builder_node( config, info )
{ }

void config_builder_list_node::add_element( config_builder_node const& node ) const
{
	list_element_node_info* const n = m_config.allocator( ).allocate( sizeof(list_element_node_info) );

	n->m_value = (u32)m_config.allocator( ).index_of( node.m_info );
	n->m_next = 0;

	u32 const n_offset = (u32)m_config.allocator( ).index_of( n );

	if ( m_info->m_list_object.current == 0 )
	{
		m_info->m_list_object.first = n_offset;
		m_info->m_list_object.current = n_offset;
	}
	else
	{
		list_element_node_info* const curr = (list_element_node_info*)( m_config.allocator( ).data( ) + m_info->m_list_object.current );
		curr->m_next = n_offset;
		m_info->m_list_object.current = n_offset;
	}
}


config_builder_object_node::config_builder_object_node( config_builder& config, node_info& info ) :
	config_builder_node( config, info )
{ }

void config_builder_object_node::add_element( pcstr const key, config_builder_node const& node ) const
{
	uptr const str_size = strings::length( key ) + 1;

	object_element_node_info* const n = m_config.allocator( ).allocate( math::align_up( sizeof(object_element_node_info) + str_size, sizeof(u64) ) );

	n->m_value = (u32)m_config.allocator( ).index_of( node.m_info );
	n->m_next = 0;
	strings::copy_n( n->m_key, key, str_size );

	u32 const n_offset = (u32)m_config.allocator( ).index_of( n );

	if ( m_info->m_list_object.current == 0 )
	{
		m_info->m_list_object.first = n_offset;
		m_info->m_list_object.current = n_offset;
	}
	else
	{
		list_element_node_info* const curr = (list_element_node_info*)( m_config.allocator( ).data( ) + m_info->m_list_object.current );
		curr->m_next = n_offset;
		m_info->m_list_object.current = n_offset;
	}
}


config_builder::config_builder( pvoid const memory, uptr const memory_size )
{
	create( memory, memory_size );
}

void config_builder::create( pvoid const memory, uptr const memory_size )
{
	m_allocator.create( memory, memory_size );
	clear( );
}

void config_builder::clear( )
{
	m_allocator.clear( );
	create_object( );
}

config_builder_node config_builder::create_bool( bool const value )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );
	n->m_type = config_node_type_bool;
	n->m_bool = value;
	return config_builder_node( *this, *n );
}

config_builder_node config_builder::create_int( s64 const value )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );
	n->m_type = config_node_type_int;
	n->m_int = value;
	return config_builder_node( *this, *n );
}

config_builder_node config_builder::create_float( double const value )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );
	n->m_type = config_node_type_float;
	n->m_float = value;
	return config_builder_node( *this, *n );
}

config_builder_node config_builder::create_string( pcstr const value )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );

	uptr const length = strings::length( value );
	pstr const str = m_allocator.allocate( math::align_up( length + 1, sizeof(u64) ) );

	n->m_type = config_node_type_string;
	n->m_str.offset = (u32)m_allocator.index_of( str );
	n->m_str.length = (u32)length;
	strings::copy_n( str, value, length + 1 );

	return config_builder_node( *this, *n );
}

config_builder_vector_node config_builder::create_vector( u32 const size )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );

	u32* const table = m_allocator.allocate( math::align_up( size * sizeof(u32), sizeof(u64) ) );

	for ( u32 i = 0; i < size; ++i )
		table[i] = 0;

	n->m_type = config_node_type_vector;
	n->m_vector.first = (u32)m_allocator.index_of( table );
	n->m_vector.count = size;

	return config_builder_vector_node( *this, *n );
}

config_builder_list_node config_builder::create_list( )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );

	n->m_type = config_node_type_list;
	n->m_list_object.first = 0;
	n->m_list_object.current = 0;

	return config_builder_list_node( *this, *n );
}

config_builder_object_node config_builder::create_object( )
{
	node_info* const n = m_allocator.allocate( sizeof(node_info) );

	n->m_type = config_node_type_object;
	n->m_list_object.first = 0;
	n->m_list_object.current = 0;

	return config_builder_object_node( *this, *n );
}

config_builder_object_node config_builder::root( )
{
	return config_builder_object_node( *this, *(node_info*)m_allocator.data( ) );
}

linear_allocator& config_builder::allocator( )
{
	return m_allocator;
}

void config_builder::serialize( lib::text_writer& w ) const
{
	node_info& n = *(node_info*)m_allocator.data( );

	for ( u32 it = n.m_list_object.first; it != 0; )
	{
		object_element_node_info& object_el = *(object_element_node_info*)( m_allocator.data( ) + it );
		node_info& element = *(node_info*)( m_allocator.data( ) + object_el.m_value );

		w.write_str( object_el.m_key );
		w.write_str( " = " );
		serialize_node( w, element, 0 );

		w.write_str( ";\n" );

		it = object_el.m_next;
	}
}

void config_builder::serialize_node( lib::text_writer& w, node_info& n, u32 const identation ) const
{
	switch ( n.m_type )
	{
		case config_node_type_bool:
		{
			w.write_str( n.m_bool ? "true" : "false" );
			break;
		}

		case config_node_type_int:
		{
			w.write( n.m_int );
			break;
		}

		case config_node_type_float:
		{
			w.write( n.m_float );
			break;
		}

		case config_node_type_string:
		{
			w.write_str( m_allocator.data( ) + n.m_str.offset );
			break;
		}

		case config_node_type_vector:
		{
			if ( n.m_vector.count == 0 )
			{
				w.write_str( "( )" );
				break;
			}

			u32* const table = m_allocator.data( ) + n.m_vector.first;

			node_info& first = *(node_info*)( m_allocator.data( ) + table[0] );

			bool const break_lines = first.m_type > config_node_type_string;
			s32 const element_identation = break_lines ? identation + 1 : -1;
			u32 const child_identation = break_lines ? identation + 1 : 0;

			w.write_str( "(" );

			for ( u32 i = 0; i < n.m_vector.count - 1; ++i )
			{
				add_identation( w, element_identation );

				node_info& element = *(node_info*)( m_allocator.data( ) + table[i] );
				serialize_node( w, element, child_identation );

				w.write_str( "," );
			}

			add_identation( w, element_identation );

			node_info& last = *(node_info*)( m_allocator.data( ) + table[n.m_vector.count - 1] );
			serialize_node( w, last, child_identation );

			add_identation( w, child_identation - 1 );
			w.write_str( ")" );

			break;
		}

		case config_node_type_list:
		{
			if ( n.m_list_object.first == 0 )
			{
				w.write_str( "[ ]" );
				break;
			}

			node_info* const first = m_allocator.data( ) + n.m_list_object.first;
			
			bool const break_lines = first->m_type > config_node_type_string;
			s32 const element_identation = break_lines ? identation + 1 : -1;
			u32 const child_identation = break_lines ? identation + 1 : 0;

			w.write_str( "[" );

			for ( u32 it = n.m_list_object.first; it != n.m_list_object.current; )
			{
				add_identation( w, element_identation );

				list_element_node_info& list_el = *(list_element_node_info*)( m_allocator.data( ) + it );
				node_info& element = *(node_info*)( m_allocator.data( ) + list_el.m_value );
				serialize_node( w, element, child_identation );

				w.write_str( "," );

				it = list_el.m_next;
			}

			add_identation( w, element_identation );

			list_element_node_info& last_list_el = *(list_element_node_info*)( m_allocator.data( ) + n.m_list_object.current );
			node_info& last_element = *(node_info*)( m_allocator.data( ) + last_list_el.m_value );
			serialize_node( w, last_element, child_identation );

			add_identation( w, child_identation - 1 );
			w.write_str( "]" );

			break;
		}

		case config_node_type_object:
		{
			if ( n.m_list_object.first == 0 )
			{
				w.write_str( "{ }" );
				break;
			}

			u32 const child_identation = identation + 1;

			w.write_str( "{" );

			for ( u32 it = n.m_list_object.first; it != n.m_list_object.current; )
			{
				add_identation( w, child_identation );

				object_element_node_info& object_el = *(object_element_node_info*)( m_allocator.data( ) + it );
				node_info& element = *(node_info*)( m_allocator.data( ) + object_el.m_value );

				w.write_str( object_el.m_key );
				w.write_str( " = " );
				serialize_node( w, element, child_identation );

				w.write_str( "," );

				it = object_el.m_next;
			}

			add_identation( w, child_identation );
			object_element_node_info& last_object_el = *(object_element_node_info*)( m_allocator.data( ) + n.m_list_object.current );
			node_info& last_element = *(node_info*)( m_allocator.data( ) + last_object_el.m_value );
			w.write_str( last_object_el.m_key );
			w.write_str( " = " );
			serialize_node( w, last_element, child_identation );

			add_identation( w, identation );
			w.write_str( "}" );

			break;
		}

		default:
			UNREACHABLE_CODE
	}
}

void config_builder::add_identation( lib::text_writer& w, s32 const size ) const
{
	if ( size < 0 )
		w.write_str( " " );
	else
	{
		u32 const identation_step = 4;
		w.write_mask( "\n%*s", math::max( size * identation_step, 1u ), " " );
	}
}

} // namespace utils