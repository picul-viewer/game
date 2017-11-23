#ifndef __core_embedded_list_h_included_
#define __core_embedded_list_h_included_

template<typename T, T* (T::*Next)>
class embedded_list
{
public:
	embedded_list( );
	~embedded_list( ) = default;

	void insert( T* object );
	void remove( T* object );
	void clear( );

	T* begin( ) const;
	T* end( ) const;

	T* first( ) const;
	T* get_next( T* object ) const;

	template<typename Pred>
	void for_each( Pred const& functor );

protected:
	T* m_first;
};

template<typename T, T* (T::*Prev), T* (T::*Next)>
class embedded_twosided_list : public embedded_list<T, Next>
{
public:
	embedded_twosided_list( ) = default;
	~embedded_twosided_list( ) = default;

	void insert( T* object );
	void remove( T* object );
};

template<typename T, T* (T::*Prev), T* (T::*Next)>
class embedded_twosided_reversable_list : public embedded_twosided_list<T, Prev, Next>
{
public:
	embedded_twosided_reversable_list( );
	~embedded_twosided_reversable_list( ) = default;

	void insert( T* object );
	void remove( T* object );
	void clear( );

	T* rbegin( ) const;
	T* rend( ) const;

	T* last( ) const;
	T* get_prev( T* object ) const;
	
	template<typename Pred>
	void reverse_for_each( Pred const& functor );

protected:
	T* m_first;
	T* m_last;
};

#include "embedded_list_inline.h"

#endif // #ifndef __core_embedded_list_h_included_
