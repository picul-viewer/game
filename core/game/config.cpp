#include "config.h"

config::config( pointer data, uptr size ) :
	m_begin		( data ),
#ifdef DEBUG
	m_end		( data + size ),
#endif // #ifdef DEBUG
	m_pointer	( data )
{ }

void config::set( pointer data, uptr size )
{
	m_begin		= data;
#ifdef DEBUG
	m_end		= data + size;
#endif // #ifdef DEBUG
	m_pointer	= data;
}

void config::move( ptr size )
{
	m_pointer += size;
	ASSERT( ( m_pointer >= m_begin ) && ( m_pointer < m_end ) );
}


config_builder::config_builder( pointer data, uptr max_size ) :
	m_begin		( data ),
#ifdef DEBUG
	m_end		( data + max_size ),
#endif // #ifdef DEBUG
	m_pointer	( data )
{ }

void config_builder::set( pointer data, uptr max_size )
{
	m_begin		= data;
#ifdef DEBUG
	m_end		= data + max_size;
#endif // #ifdef DEBUG
	m_pointer	= data;
}

pointer config_builder::data( ) const
{
	return m_begin;
}

uptr config_builder::size( ) const
{
	return m_pointer - m_begin;
}