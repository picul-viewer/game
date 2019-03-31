#ifndef GUARD_RENDER_EFFECT_INLINE_H_INCLUDED
#define GUARD_RENDER_EFFECT_INLINE_H_INCLUDED

namespace render {

void technique::set_stencil_ref( u8 in_stencil_ref )
{
	m_stencil_ref = in_stencil_ref;
}

void technique::set_blend_factor( math::float4 in_blend_factor )
{
	m_blend_factor = in_blend_factor;
}

void technique::set_sample_mask( u32 in_sample_mask )
{
	m_sample_mask = in_sample_mask;
}

u8 technique::get_stencil_ref(  ) const
{
	return m_stencil_ref;
}

math::float4 technique::get_blend_factor(  ) const
{
	return m_blend_factor;
}

u32 technique::get_sample_mask(  ) const
{
	return m_sample_mask;
}


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
	ASSERT_CMP( in_index, <, TechniqueCount );
	return m_techniques[in_index];
}

template<uptr TechniqueCount>
technique const& effect<TechniqueCount>::operator[]( uptr in_index ) const
{
	ASSERT_CMP( in_index, <, TechniqueCount );
	return m_techniques[in_index];
}

template<uptr TechniqueCount>
void effect<TechniqueCount>::apply( uptr in_index ) const
{
	ASSERT_CMP( in_index, <, TechniqueCount );
	m_techniques[in_index].apply( );
}

} // namespace render

#endif // #ifndef GUARD_RENDER_EFFECT_INLINE_H_INCLUDED