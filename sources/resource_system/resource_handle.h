#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_HANDLE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_HANDLE_H_INCLUDED

#include "shared_resource_ptr.h"

#define DECLARE_DEFAULT_RESOURCE_HANDLE( Resource, IdType )								\
																						\
class Resource;																			\
																						\
class Resource##_handle																	\
{																						\
public:																					\
	typedef IdType value_type;															\
																						\
	static IdType const invalid;														\
																						\
public:																					\
	Resource##_handle( );																\
	~Resource##_handle( );																\
																						\
	Resource##_handle( Resource##_handle const& in_handle ) = delete;					\
	Resource##_handle& operator=( Resource##_handle const& in_handle ) = default;		\
																						\
	Resource##_handle( Resource##_handle&& in_handle );									\
	Resource##_handle& operator=( Resource##_handle&& in_handle );						\
																						\
	operator IdType( ) const;															\
																						\
private:																				\
	friend class Resource;																\
	friend class default_resource_ptr<Resource>;										\
	friend class ::resource_system::queried_resources;									\
																						\
	Resource##_handle( pvoid const in_ptr );											\
	Resource##_handle( IdType const in_id );											\
																						\
private:																				\
	IdType m_id;																		\
																						\
};																						\


#define DEFINE_DEFAULT_RESOURCE_HANDLE( Resource, IdType, Invalid )						\
																						\
IdType const Resource##_handle::invalid = (IdType)Invalid;								\
																						\
Resource##_handle::Resource##_handle( ) :												\
	m_id( invalid )																		\
{ }																						\
																						\
Resource##_handle::Resource##_handle( pvoid const in_ptr ) :							\
	m_id( Resource::to_handle_value( (Resource*)in_ptr ) )								\
{ }																						\
																						\
Resource##_handle::Resource##_handle( IdType const in_id ) :							\
	m_id( in_id )																		\
{ }																						\
																						\
Resource##_handle::Resource##_handle( Resource##_handle&& in_handle )					\
{																						\
	m_id = in_handle.m_id;																\
	in_handle.m_id = invalid;															\
}																						\
																						\
Resource##_handle& Resource##_handle::operator=( Resource##_handle&& in_handle )		\
{																						\
	lib::swap( m_id, in_handle.m_id );													\
	return *this;																		\
}																						\
																						\
Resource##_handle::~Resource##_handle( )												\
{																						\
	if ( m_id != invalid )																\
		Resource::from_handle( *this )->delete_resource( );								\
}																						\
																						\
Resource##_handle::operator IdType( ) const												\
{																						\
	return m_id;																		\
}																						\


#define DECLARE_SHARED_RESOURCE_HANDLE( Resource, IdType )								\
																						\
class Resource;																			\
																						\
class Resource##_handle																	\
{																						\
public:																					\
	typedef IdType value_type;															\
																						\
	static IdType const invalid;														\
																						\
public:																					\
	Resource##_handle( );																\
	~Resource##_handle( );																\
																						\
	Resource##_handle( Resource##_handle const& in_handle );							\
	Resource##_handle& operator=( Resource##_handle const& in_handle );					\
																						\
	Resource##_handle( Resource##_handle&& in_handle );									\
	Resource##_handle& operator=( Resource##_handle&& in_handle );						\
																						\
	u32 add_ref( );																		\
	u32 release( );																		\
																						\
	IdType id( ) const;																	\
	u32 ref_count( ) const;																\
																						\
	operator IdType( ) const;															\
																						\
private:																				\
	friend class Resource;																\
	friend class shared_resource_ptr<Resource>;											\
	friend class ::resource_system::queried_resources;									\
																						\
	Resource##_handle( pvoid const in_ptr );											\
	Resource##_handle( IdType const in_id );											\
																						\
private:																				\
	IdType m_id;																		\
																						\
};																						\


#define DEFINE_SHARED_RESOURCE_HANDLE( Resource, IdType, Invalid )						\
																						\
IdType const Resource##_handle::invalid = (IdType)Invalid;								\
																						\
Resource##_handle::Resource##_handle( ) :												\
	m_id( invalid )																		\
{ }																						\
																						\
Resource##_handle::Resource##_handle( pvoid const in_ptr ) :							\
	m_id( Resource::to_handle_value( (Resource*)in_ptr ) )								\
{ }																						\
																						\
Resource##_handle::Resource##_handle( IdType const in_id ) :							\
	m_id( in_id )																		\
{ }																						\
																						\
Resource##_handle::Resource##_handle( Resource##_handle const& in_handle ) :			\
	m_id( in_handle.m_id )																\
{																						\
	add_ref( );																			\
}																						\
																						\
Resource##_handle& Resource##_handle::operator=( Resource##_handle const& in_handle )	\
{																						\
	m_id = in_handle.m_id;																\
	add_ref( );																			\
	return *this;																		\
}																						\
																						\
Resource##_handle::Resource##_handle( Resource##_handle&& in_handle )					\
{																						\
	m_id = in_handle.m_id;																\
	in_handle.m_id = invalid;															\
}																						\
																						\
Resource##_handle& Resource##_handle::operator=( Resource##_handle&& in_handle )		\
{																						\
	lib::swap( m_id, in_handle.m_id );													\
	return *this;																		\
}																						\
																						\
Resource##_handle::~Resource##_handle( )												\
{																						\
	if ( m_id != invalid )																\
		Resource::from_handle( *this )->release( );										\
}																						\
																						\
u32 Resource##_handle::add_ref( )														\
{																						\
	return Resource::from_handle( *this )->add_ref( );									\
}																						\
																						\
u32 Resource##_handle::release( )														\
{																						\
	return Resource::from_handle( *this )->release( );									\
}																						\
																						\
IdType Resource##_handle::id( ) const													\
{																						\
	return Resource::from_handle( *this )->id( );										\
}																						\
																						\
u32 Resource##_handle::ref_count( ) const												\
{																						\
	return Resource::from_handle( *this )->ref_count( );								\
}																						\
																						\
Resource##_handle::operator IdType( ) const												\
{																						\
	return m_id;																		\
}																						\


#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_HANDLE_H_INCLUDED