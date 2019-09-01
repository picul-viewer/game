#ifndef GUARD_UTILS_CONFIG_READER_H_INCLUDED
#define GUARD_UTILS_CONFIG_READER_H_INCLUDED

#include <types.h>
#include <lib/linear_allocator.h>
#include "config_common.h"

namespace utils {

class config_reader_node
{
public:
	bool valid( ) const;
	config_node_type type( ) const;

	bool	as_bool		( ) const;
	s64		as_int		( ) const;
	double	as_float	( ) const;
	pcstr	as_string	( ) const;

	uptr size( ) const;

	config_reader_node operator( )( uptr const index ) const;

	config_reader_node operator[]( uptr const index ) const;

	config_reader_node operator[]( pcstr const key ) const;

private:
	friend class config_reader;
	friend struct node_info;

	config_reader_node( config_reader const& config, node_info const& info );

	node_info& invalid( ) const;

private:
	config_reader const& m_config;
	node_info const& m_info;

};

struct config_error_info
{
	uptr position;
	pcstr message;
	u32 line_index;
	u32 line_position;
};

class config_reader
{
public:
	config_reader( ) = default;

	config_reader( pvoid const memory, uptr const memory_size );
	void create( pvoid const memory, uptr const memory_size );

	bool parse( pcstr const s );

	pcstr source( ) const;
	config_error_info const& error_info( ) const;

	void log_error( pcstr const config_name ) const;

	config_reader_node root( ) const;

private:
	friend class config_reader_node;

	pointer arena( ) const;

private:
	bool parse_entity( pcstr& str, u32& value );
	bool parse_scalar( pcstr& str, u32& value );
	bool parse_vector( pcstr& str, u32& value );
	bool parse_list( pcstr& str, u32& value );
	bool parse_object( pcstr& str, u32& value );

	bool build_hash_table( pcstr const str, u32 const* const object_props, u32 const* const object_prop_hashes, u32 const prop_count, u32& table, u32& table_size );

	void error( pcstr const str, pcstr const message );

private:
	linear_allocator m_allocator;
	pcstr m_source;
	config_error_info m_error;

};

} // namespace utils

#endif // #ifndef GUARD_UTILS_CONFIG_READER_H_INCLUDED