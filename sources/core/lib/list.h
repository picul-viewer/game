#ifndef GUARD_CORE_LIST_H_INCLUDED
#define GUARD_CORE_LIST_H_INCLUDED

#include <types.h>
#include "algorithms.h"

namespace lib {

template<typename T, typename Linker>
class list
{
public:
	class iterator;
	class const_iterator;

	typedef T value_type;

public:
	void create( );

	void insert_front( T* const object );
	void insert_after( T* const object, T* const prev );
	void remove( T* const object );
	void clear( );

	iterator begin( );
	const_iterator begin( ) const;
	iterator end( );
	const_iterator end( ) const;

protected:
	T* m_first;

};

template<typename T, typename Linker>
class list<T, Linker>::iterator : public lib::iterator<forward_iterator_tag, T>
{
public:
	iterator( T* const object );

	iterator& operator++( );
	iterator operator++( int );

	bool operator==( iterator const other ) const;
	bool operator!=( iterator const other ) const;

	T& operator*( ) const;
	T* operator->( ) const;

private:
	T* m_object;

};

template<typename T, typename Linker>
class list<T, Linker>::const_iterator : public lib::iterator<forward_iterator_tag, T const>
{
public:
	const_iterator( T const* const object );

	const_iterator& operator++( );
	const_iterator operator++( int );

	bool operator==( const_iterator const other ) const;
	bool operator!=( const_iterator const other ) const;

	T const& operator*( ) const;
	T const* operator->( ) const;

private:
	T const* m_object;

};

template<typename T, T* (T::*Next)>
struct intrusive_list_linker
{
	static void set_next( T* const object, T* const next );
	static T* get_next( T* const object );
	static T const* get_next( T const* const object );
};

template<typename T, T* (T::*Next)>
using intrusive_list = list<T, intrusive_list_linker<T, Next>>;

} // namespace lib

#include "list_inline.h"

#endif // #ifndef GUARD_CORE_LIST_H_INCLUDED