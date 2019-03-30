#ifndef __core_fixed_array_h_included_
#define __core_fixed_array_h_included_

#include <types.h>
#include "memory_block.h"

namespace lib {

template<typename T, uptr Size>
class fixed_array
{
public:
	typedef T value_type;

	fixed_array( ) = default;

	T* data( );
	T const* data( ) const;
	uptr size( ) const;

	T* begin( );
	T const* begin( ) const;
	T* end( );
	T const* end( ) const;

	T& at( uptr const index );
	T const& at( uptr const index ) const;
	T& operator[]( uptr const index );
	T const& operator[]( uptr const index ) const;
	
	template<typename Pred>
	void for_each( Pred const& functor ) const;

private:
	memory_block<sizeof(T)> m_data[Size];

};

} // namespace lib

#include "fixed_array_inline.h"

#endif // #ifndef __core_fixed_array_h_included_