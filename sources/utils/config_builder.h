#ifndef GUARD_UTILS_CONFIG_BUILDER_H_INCLUDED
#define GUARD_UTILS_CONFIG_BUILDER_H_INCLUDED

#include <types.h>
#include <lib/linear_allocator.h>
#include <lib/text_writer.h>
#include "config_common.h"

namespace utils {

class config_builder_node
{
protected:
	friend class config_builder;
	friend struct node_info;

	config_builder_node( config_builder& config, node_info& info );

	node_info& invalid( ) const;

protected:
	friend class config_builder_vector_node;
	friend class config_builder_list_node;
	friend class config_builder_object_node;

	config_builder& m_config;
	node_info* m_info;

};

class config_builder_vector_node : public config_builder_node
{
public:
	config_builder_vector_node( config_builder& config, node_info& info );

	void set_element( u32 const index, config_builder_node const& node ) const;

};

class config_builder_list_node : public config_builder_node
{
public:
	config_builder_list_node( config_builder& config, node_info& info );

	void add_element( config_builder_node const& node ) const;

};

class config_builder_object_node : public config_builder_node
{
public:
	config_builder_object_node( config_builder& config, node_info& info );

	void add_element( pcstr const key, config_builder_node const& node ) const;

};

class config_builder
{
public:
	config_builder( ) = default;

	config_builder( pvoid const memory, uptr const memory_size );
	void create( pvoid const memory, uptr const memory_size );

	void clear( );

	config_builder_node create_bool( bool const value );
	config_builder_node create_int( s64 const value );
	config_builder_node create_float( double const value );
	config_builder_node create_string( pcstr const value );
	config_builder_vector_node create_vector( u32 const size );
	config_builder_list_node create_list( );
	config_builder_object_node create_object( );

	config_builder_object_node root( );

	void serialize( lib::text_writer& w ) const;

private:
	friend class config_builder_node;
	friend class config_builder_vector_node;
	friend class config_builder_list_node;
	friend class config_builder_object_node;

	linear_allocator& allocator( );

private:
	void serialize_node( lib::text_writer& w, node_info& n, u32 const identation ) const;
	void add_identation( lib::text_writer& w, s32 const size ) const;

private:
	linear_allocator m_allocator;

};

} // namespace utils

#endif // #ifndef GUARD_UTILS_CONFIG_BUILDER_H_INCLUDED