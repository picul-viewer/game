#ifndef GUARD_RENDER_CONSTANT_BUFFER_INLINE_H_INCLUDED
#define GUARD_RENDER_CONSTANT_BUFFER_INLINE_H_INCLUDED

namespace render {

template<typename Data>
void constant_buffer::update( Data const& in_data ) const
{
	update( &in_data, sizeof(Data), 0 );
}

void constant_buffer::set( ID3D11Buffer* in_buffer )
{
	m_buffer.set( in_buffer );
}

ID3D11Buffer* const& constant_buffer::get( ) const
{
	return m_buffer.get( );
}

} // namespace render

#endif // #ifndef GUARD_RENDER_CONSTANT_BUFFER_INLINE_H_INCLUDED