#ifndef __render_constant_buffer_inline_h_included_
#define __render_constant_buffer_inline_h_included_

namespace render {

template<typename Data>
void constant_buffer::update( Data const& in_data ) const
{
	update( &in_data, 0, sizeof(Data) );
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

#endif // #ifndef __render_constant_buffer_inline_h_included_