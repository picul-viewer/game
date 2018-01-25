#ifndef __render_effect_inline_h_included_
#define __render_effect_inline_h_included_

namespace render {

template<uptr TechniqueCount>
void effect<TechniqueCount>::create( )
{
	for ( uptr i = 0; i < TechniqueCount; ++i )
		m_techniques[i].create( );
}

template<uptr TechniqueCount>
void effect<TechniqueCount>::destroy( )
{
	for ( uptr i = 0; i < TechniqueCount; ++i )
		m_techniques[i].destroy( );
}

template<uptr TechniqueCount>
technique& effect<TechniqueCount>::operator[]( uptr in_index )
{
	ASSERT( in_index < TechniqueCount );
	return m_techniques[in_index];
}

template<uptr TechniqueCount>
technique const& effect<TechniqueCount>::operator[]( uptr in_index ) const
{
	ASSERT( in_index < TechniqueCount );
	return m_techniques[in_index];
}

template<uptr TechniqueCount>
void effect<TechniqueCount>::apply( uptr in_index ) const
{
	ASSERT( in_index < TechniqueCount );
	m_techniques[in_index].apply( );
}

} // namespace render

#endif // #ifndef __render_effect_inline_h_included_