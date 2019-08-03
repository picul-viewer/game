#ifndef GUARD_UTILS_CMD_ARGS_INLINE_INCLUDED
#define GUARD_UTILS_CMD_ARGS_INLINE_INCLUDED

namespace utils {

template<typename T>
cmd_arg<T>::cmd_arg( pcstr const key, T const& default_value ) :
	m_data( default_value )
{
	parse_cmd_line( key, &on_found, this );
}

template<typename T>
cmd_arg<T>::operator T( ) const
{
	return m_data;
}

template<typename T>
T cmd_arg<T>::value( ) const
{
	return m_data;
}

template<typename T>
void cmd_arg<T>::on_found( pcstr const str, pointer const this_ptr )
{
	cmd_arg<T>* const ptr = this_ptr;
	pcstr s = str;

	parse_value( s, ptr->m_data );
}

} // namespace utils

#endif // #ifndef GUARD_UTILS_CMD_ARGS_INLINE_INCLUDED