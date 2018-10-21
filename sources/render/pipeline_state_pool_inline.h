#ifndef __render_pipeline_state_pool_inline_h_included_
#define __render_pipeline_state_pool_inline_h_included_

namespace render {

namespace __render_pipeline_state_pool_detail {

template<typename DataType>
typename DataType::data_type create( typename DataType::cook_type const& in_cook, DataType* in_data, u32 in_count );

template dss_data::data_type create<dss_data>( typename dss_data::cook_type const& in_cook, dss_data* in_data, u32 in_count );
template bs_data::data_type create<bs_data>( typename bs_data::cook_type const& in_cook, bs_data* in_data, u32 in_count );
template rs_data::data_type create<rs_data>( typename rs_data::cook_type const& in_cook, rs_data* in_data, u32 in_count );

} // namespace __render_pipeline_state_pool_detail

template<u32 DS_max, u32 B_max, u32 R_max>
depth_stencil_state pipeline_state_pool<DS_max, B_max, R_max>::create_depth_stencil( depth_stencil_state::cook const& in_cook )
{
	return __render_pipeline_state_pool_detail::create( in_cook, m_depth_stencil_data, DS_max );
}

template<u32 DS_max, u32 B_max, u32 R_max>
blend_state pipeline_state_pool<DS_max, B_max, R_max>::create_blend( blend_state::cook const& in_cook )
{
	return __render_pipeline_state_pool_detail::create( in_cook, m_blend_data, B_max );
}

template<u32 DS_max, u32 B_max, u32 R_max>
rasterizer_state pipeline_state_pool<DS_max, B_max, R_max>::create_rasterizer( rasterizer_state::cook const& in_cook )
{
	return __render_pipeline_state_pool_detail::create( in_cook, m_rasterizer_data, R_max );
}

template<u32 DS_max, u32 B_max, u32 R_max>
void pipeline_state_pool<DS_max, B_max, R_max>::destroy( )
{
	for ( u32 i = 0; ( i < DS_max ) && ( m_depth_stencil_data[i].state.get( ) != nullptr ); ++i )
		m_depth_stencil_data[i].state.destroy( );
	for ( u32 i = 0; ( i < B_max ) && ( m_blend_data[i].state.get( ) != nullptr ); ++i )
		m_blend_data[i].state.destroy( );
	for ( u32 i = 0; ( i < R_max ) && ( m_rasterizer_data[i].state.get( ) != nullptr ); ++i )
		m_rasterizer_data[i].state.destroy( );
}

} // namespace render

#endif // #ifndef __render_pipeline_state_pool_inline_h_included_