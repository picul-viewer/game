#ifndef GUARD_CORE_REVERSABLE_DOUBLE_LINKED_LIST_H_INCLUDED
#define GUARD_CORE_REVERSABLE_DOUBLE_LINKED_LIST_H_INCLUDED

#include <types.h>
#include "double_linked_list.h"

namespace lib {

template<typename T, typename Linker>
class reversable_double_linked_list : protected double_linked_list<T, Linker>
{
private:
	typedef double_linked_list<T, Linker> parent_type;

public:
	typedef parent_type::iterator iterator;
	typedef parent_type::const_iterator const_iterator;
	class reverse_iterator;
	class const_reverse_iterator;

	typedef T value_type;

public:
	void create( );

	void insert_front( T* const object );
	void insert_back( T* const object );
	using parent_type::insert_after;
	using parent_type::insert_before;

	void remove( T* const object );
	void clear( );

	using parent_type::begin;
	using parent_type::end;

	reverse_iterator rbegin( );
	const_reverse_iterator rbegin( ) const;
	reverse_iterator rend( );
	const_reverse_iterator rend( ) const;

private:
	T* m_last;

};

template<typename T, typename Linker>
class reversable_double_linked_list<T, Linker>::reverse_iterator
{
public:
	reverse_iterator( T* const object );

	reverse_iterator& operator++( );
	reverse_iterator operator++( int );

	bool operator==( reverse_iterator const other ) const;
	bool operator!=( reverse_iterator const other ) const;

	T* operator*( ) const;
	T* operator->( ) const;

private:
	T* m_object;

};

template<typename T, typename Linker>
class reversable_double_linked_list<T, Linker>::const_reverse_iterator
{
public:
	const_reverse_iterator( T const* const object );

	const_reverse_iterator& operator++( );
	const_reverse_iterator operator++( int );

	bool operator==( const_reverse_iterator const other ) const;
	bool operator!=( const_reverse_iterator const other ) const;

	T const* operator*( ) const;
	T const* operator->( ) const;

private:
	T const* m_object;

};

} // namespace lib

#include "reversable_double_linked_list_inline.h"

#endif // #ifndef GUARD_CORE_REVERSABLE_DOUBLE_LINKED_LIST_H_INCLUDED