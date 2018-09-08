#ifndef __core_fixed_array_h_included_
#define __core_fixed_array_h_included_

#include <array>

template<typename T, uptr Size>
class fixed_array : public std::array<T, Size>
{ };

#endif // #ifndef __core_fixed_array_h_included_