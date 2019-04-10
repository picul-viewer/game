#ifndef GUARD_CORE_DOUBLE_LINKED_LIST_H_INCLUDED
#define GUARD_CORE_DOUBLE_LINKED_LIST_H_INCLUDED

#include <types.h>
#include "list.h"

namespace lib {

template<typename T, typename Linker>
class double_linked_list : protected list<T, Linker>
{
private:
	typedef list<T, Linker> parent_type;

public:
	class iterator;
	class const_iterator;

	typedef T value_type;

public:
	using parent_type::create;

	void insert_front( T* const object );
	void insert_after( T* const object, T* const prev );
	void insert_before( T* const object, T* const next );
	void remove( T* const object );
	using parent_type::clear;

	iterator begin( );
	const_iterator begin( ) const;
	iterator end( );
	const_iterator end( ) const;

};

template<typename T, typename Linker>
class double_linked_list<T, Linker>::iterator : public lib::iterator<bidirectional_iterator_tag, T>
{
public:
	iterator( T* const object );

	iterator& operator++( );
	iterator operator++( int );

	iterator& operator--( );
	iterator operator--( int );

	bool operator==( iterator const other ) const;
	bool operator!=( iterator const other ) const;

	T& operator*( ) const;
	T* operator->( ) const;

private:
	T* m_object;

};

template<typename T, typename Linker>
class double_linked_list<T, Linker>::const_iterator : public lib::iterator<bidirectional_iterator_tag, T const>
{
public:
	const_iterator( T const* const object );

	const_iterator& operator++( );
	const_iterator operator++( int );

	const_iterator& operator--( );
	const_iterator operator--( int );

	bool operator==( const_iterator const other ) const;
	bool operator!=( const_iterator const other ) const;

	T const& operator*( ) const;
	T const* operator->( ) const;

private:
	T const* m_object;

};

template<typename T, T* (T::*Prev), T* (T::*Next)>
struct intrusive_double_linked_list_linker
{
	static void set_prev( T* const object, T* const next );
	static T* get_prev( T* const object );
	static T const* get_prev( T const* const object );
	
	static void set_next( T* const object, T* const next );
	static T* get_next( T* const object );
	static T const* get_next( T const* const object );
};

template<typename T, T* (T::*Prev), T* (T::*Next)>
using intrusive_double_linked_list = double_linked_list<T, intrusive_double_linked_list_linker<T, Prev, Next>>;

} // namespace lib

#include "double_linked_list_inline.h"

#endif // #ifndef GUARD_CORE_DOUBLE_LINKED_LIST_H_INCLUDED