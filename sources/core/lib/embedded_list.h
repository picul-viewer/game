#ifndef __core_embedded_list_h_included_
#define __core_embedded_list_h_included_

#include <types.h>

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
	void for_each( Pred const& functor ) const;

protected:
	T* m_first;
};

template<typename T, T* (T::*Next), T* (T::*Prev)>
class embedded_twosided_list : public embedded_list<T, Next>
{
public:
	embedded_twosided_list( ) = default;
	~embedded_twosided_list( ) = default;

	void insert( T* object );
	void remove( T* object );
};

template<typename T, T* (T::*Next), T* (T::*Prev)>
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
	void reverse_for_each( Pred const& functor ) const;

protected:
	T* m_last;
};

// Typeless structures assume needed members to have defined offset
// NOTE: these structures are incredibly unsafe!
class embedded_typeless_list
{
public:
	inline embedded_typeless_list( );
	~embedded_typeless_list( ) = default;

	inline void insert( pointer object );
	inline void remove( pointer object );
	inline void clear( );

	inline pointer begin( ) const;
	inline pointer end( ) const;

	inline pointer first( ) const;
	inline pointer get_next( pointer object ) const;

	template<typename Pred>
	inline void for_each( Pred const& functor ) const;

protected:
	struct node
	{
		pointer next;
		// Other data, which is not handling by list
	};

	pointer m_first;
};

class embedded_typeless_twosided_list : embedded_typeless_list
{
public:
	embedded_typeless_twosided_list( ) = default;
	~embedded_typeless_twosided_list( ) = default;

	inline void insert( pointer object );
	inline void remove( pointer object );

protected:
	struct node
	{
		pointer next;
		pointer prev;
		// Other data, which is not handling by list
	};
};

class embedded_typeless_twosided_reversable_list : public embedded_typeless_twosided_list
{
public:
	inline embedded_typeless_twosided_reversable_list( );
	~embedded_typeless_twosided_reversable_list( ) = default;

	inline void insert( pointer object );
	inline void remove( pointer object );
	inline void clear( );

	inline pointer rbegin( ) const;
	inline pointer rend( ) const;

	inline pointer last( ) const;
	inline pointer get_prev( pointer object ) const;
	
	template<typename Pred>
	inline void reverse_for_each( Pred const& functor ) const;

protected:
	pointer m_first;
	pointer m_last;
};

#include "embedded_list_inline.h"

#endif // #ifndef __core_embedded_list_h_included_
