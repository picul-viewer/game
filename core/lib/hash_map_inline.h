#ifndef __core_hash_map_inline_h_included_
#define __core_hash_map_inline_h_included_

#include <macros.h>

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::hash_map_template( )
{ }

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
template<typename TableAllocator>
hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::hash_map_template( uptr table_length, TableAllocator& table_allocator, KVStorePool& kv_pool ) :
	m_pool			( kv_pool ),
	m_table			( table_allocator.allocate( sizeof(typename KVStoreIndex::value_type) * table_length ) ),
	m_table_length	( table_length )
{
	for ( uptr i = 0; i < m_table_length; ++i )
		m_table[i] = KVStoreIndex::null_value;
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline KVStore* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::insert( K const& key, V const& value )
{
	auto hash = HashPred( )( key );

	return insert( key, hash, value );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline KVStore* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::insert( K const& key, hash_type hash, V const& value )
{
	ASSERT( HashPred( )( key ) == hash );

	auto& index = m_table[hash % m_table_length];
	KVStore* first_kv = KVStoreIndex::get( m_pool, index );

	ASSERT( find_key_in_kv_list( first_kv, key ) == nullptr );

	KVStore* new_kv = m_pool.allocate( sizeof(KVStore) );
	new ( new_kv ) KVStore( key, value );

	if ( first_kv )
	{
		auto next = first_kv->next( );
		KVStore* next_kv = KVStoreIndex::get( m_pool, next );
		auto new_index = KVStoreIndex::index_of( m_pool, new_kv );

		first_kv->set_next( new_index );
		next_kv->set_prev( new_index );
		new_kv->set_prev( index );
		new_kv->set_next( next );
	}
	else
	{
		index = KVStoreIndex::index_of( m_pool, new_kv );
		new_kv->set_next( index );
		new_kv->set_prev( index );
	}

	return new_kv;
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline V* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::find( K const& key ) const
{
	KVStore* dest = find_kv( key );

	return ( dest == nullptr ) ? nullptr : &dest->value( );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline V* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::find( K const& key, hash_type hash ) const
{
	ASSERT( HashPred( )( key ) == hash );

	KVStore* dest = find_kv( key, hash );

	return ( dest == nullptr ) ? nullptr : &dest->value( );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline KVStore* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::find_kv( K const& key ) const
{
	auto hash = HashPred( )( key );

	return find_kv( key, hash );
}
	
template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline KVStore* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::find_kv( K const& key, hash_type hash ) const
{
	ASSERT( HashPred( )( key ) == hash );

	KVStore* first = find_first_kv( hash );
	return find_key_in_kv_list( first, key );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline bool hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::remove( K const& key )
{
	auto hash = HashPred( )( key );

	return remove( key, hash );
}
	
template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline bool hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::remove( K const& key, hash_type hash )
{
	ASSERT( HashPred( )( key ) == hash );

	auto& index = m_table[hash % m_table_length];
	KVStore* first = KVStoreIndex::get( m_pool, index );

	KVStore* dest = find_key_in_kv_list( first, key );

	if ( dest == nullptr )
		return false;

	auto next = dest->next( );
	KVStore* next_kv = KVStoreIndex::get( m_pool, next );
	if ( next_kv == dest )
		index = KVStoreIndex::null_value;
	else
	{
		if ( first == dest )
			index = next;

		auto prev = dest->prev( );
		KVStore* prev_kv = KVStoreIndex::get( m_pool, prev );

		prev_kv->set_next( next );
		next_kv->set_prev( prev );
	}

	m_pool.deallocate( dest );

	return true;
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline void hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::remove_kv( KVStore* kv )
{
	if ( kv == nullptr )
		return;

	auto hash = HashPred( )( kv->key( ) );

	remove_kv( kv, hash );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline void hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::remove_kv( KVStore* kv, hash_type hash )
{
	if ( kv == nullptr )
		return;
	
	ASSERT( HashPred( )( kv->key( ) ) == hash );

	auto& index = m_table[hash % m_table_length];
	KVStore* first = KVStoreIndex::get( m_pool, index );

	auto next = kv->next( );
	KVStore* next_kv = KVStoreIndex::get( m_pool, next );
	if ( next_kv == kv )
		index = KVStoreIndex::null_value;
	else
	{
		if ( first == kv )
			index = next;

		auto prev = kv->prev( );
		KVStore* prev_kv = KVStoreIndex::get( m_pool, prev );

		prev_kv->set_next( next );
		next_kv->set_prev( prev );
	}

	m_pool.deallocate( kv );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline KVStore* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::find_first_kv( hash_type hash ) const
{
	auto const& index = m_table[hash % m_table_length];
	return KVStoreIndex::get( m_pool, index );
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline KVStore* hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::find_key_in_kv_list( KVStore* list, K const& key ) const
{
	if ( list == nullptr )
		return nullptr;

	KVStore* current = list;
	do
	{
		if ( KeyEqualPred( )( current->key( ), key ) )
			return current;

		current = KVStoreIndex::get( m_pool, current->next( ) );
	}
	while ( current != list );

	return nullptr;
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
inline void hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::clear( )
{
	for ( uptr i = 0; i < m_table_length; ++i )
	{
		auto& index = m_table[i];

		if ( index == KVStoreIndex::null_value )
			continue;

		KVStore* first_kv = KVStoreIndex::get( m_pool, index );
		KVStore* current = first_kv;

		do
		{
			KVStore* new_current = KVStoreIndex::get( m_pool, current->next( ) );
			m_pool.deallocate( current );
			current = new_current;
		}
		while ( current != first_kv );
	}

	for ( uptr i = 0; i < m_table_length; ++i )
		m_table[i] = KVStoreIndex::null_value;
}

template<typename K, typename V, typename HashPred, typename KeyEqualPred, typename KVStore, typename KVStoreIndex, typename KVStorePool>
template<typename Pred>
inline void hash_map_template<K, V, HashPred, KeyEqualPred, KVStore, KVStoreIndex, KVStorePool>::for_each( Pred const& functor ) const
{
	for ( uptr i = 0; i < m_table_length; ++i )
	{
		auto& index = m_table[i];

		if ( index == KVStoreIndex::null_value )
			continue;

		KVStore* first_kv = KVStoreIndex::get( m_pool, index );
		KVStore* current = first_kv;

		do
		{
			functor( current->key( ), current->value( ) );

			current = KVStoreIndex::get( m_pool, current->next( ) );
		}
		while ( current != first_kv );
	}
}

#endif // #ifndef __core_hash_map_inline_h_included_