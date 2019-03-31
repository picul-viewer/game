#ifndef __core_double_linked_list_h_included_
#define __core_double_linked_list_h_included_

#include <types.h>
#include "list.h"

namespace lib {

template<typename T, typename Linker>
class double_linked_list : protected list<T, Linker>
{
private:
	typedef list<T, Linker> parent_type;

public:
	typedef parent_type::iterator iterator;
	typedef parent_type::const_iterator const_iterator;

	typedef T value_type;

public:
	using parent_type::create;

	void insert_front( T* const object );
	void insert_after( T* const object, T* const prev );
	void insert_before( T* const object, T* const next );
	void remove( T* const object );
	using parent_type::clear;

	using parent_type::begin;
	using parent_type::end;

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
using intrusive_double_linked_list = list<T, intrusive_double_linked_list_linker<T, Prev, Next>>;

} // namespace lib

#include "double_linked_list_inline.h"

#endif // #ifndef __core_double_linked_list_h_included_