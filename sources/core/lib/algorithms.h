#ifndef GUARD_CORE_ALGORITHMS_H_INCLUDED
#define GUARD_CORE_ALGORITHMS_H_INCLUDED

#include <algorithm>
#include <numeric>

namespace lib {

using std::iterator;

using std::bidirectional_iterator_tag;
using std::forward_iterator_tag;
using std::input_iterator_tag;
using std::output_iterator_tag;
using std::random_access_iterator_tag;


using std::iota;
using std::sort;
using std::for_each;
using std::find;
using std::find_if;

using std::move;

template<typename T>
inline void swap( T& left, T& right )
{
	T temp = std::move( left );
	left = std::move( right );
	right = std::move( temp );
}

} // namespace lib

#endif // #ifndef GUARD_CORE_ALGORITHMS_H_INCLUDED