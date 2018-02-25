#ifndef __core_hash_map_h_included_
#define __core_hash_map_h_included_

#include <types.h>
#include "pool.h"

template<
	typename K,
	typename V,
	typename HashPred,
	typename KeyEqualPred,
	typename KVStore,
	typename KVStoreIndex,
	typename KVStorePool
>
struct hash_map_template
{
public:
	typedef KVStorePool kv_store_pool_type;

	template<typename TableAllocator>
	hash_map_template( uptr table_length, TableAllocator& table_allocator, KVStorePool& kv_pool );

	inline void insert( K const& key, V const& value );
	inline V* find( K const& key ) const;
	inline bool remove( K const& key );

	inline KVStore* find_kv( K const& key ) const;
	inline void remove_kv( KVStore* kv );

	inline void dump( )
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
				printf( "%d ", (u32)current->key( ) );

				current = KVStoreIndex::get( m_pool, current->next( ) );
			}
			while ( current != first_kv );

			printf( "\n" );
		}
	}

protected:
	template<typename Hash>
	inline KVStore* find_first_kv( Hash const& hash ) const;

	inline KVStore* find_key_in_kv_list( KVStore* list, K const& key ) const;

	KVStorePool& m_pool;
	typename KVStoreIndex::value_type* m_table;
	uptr m_table_length;
};

template<typename T, typename Hash>
struct hash_map_hash_pred_adapter
{
	typename Hash::value_type operator( )( T const& value )
	{
		return Hash( )( (pcvoid)&value, sizeof(T) );
	}
};

template<typename K>
struct hash_map_key_equal_pred_adapter
{
	bool operator( )( K const& l, K const& r )
	{
		return l == r;
	}
};

template<typename K, typename V>
struct hash_map_kv_store_16
{
	K _key;
	V _value;
	u16 _next;
	u16 _prev;

	inline hash_map_kv_store_16( K const& key, V const& value ) :
		_key	( key ),
		_value	( value )
	{ }

	inline K&	key		( ) { return _key;		}
	inline V&	value	( ) { return _value;	}
	inline u16	next	( ) { return _next;		}
	inline u16	prev	( ) { return _prev;		}
	
	inline void set_key		( K const& key		) { _key	= key;		}
	inline void set_value	( V const& value	) { _value	= value;	}
	inline void set_next	( u16 next			) { _next	= next;		}
	inline void set_prev	( u16 prev			) { _prev	= prev;		}
};

template<typename K, typename V>
struct hash_map_kv_store_32
{
	K _key;
	V _value;
	u32 _next;
	u32 _prev;

	inline hash_map_kv_store_32( K const& key, V const& value ) :
		_key	( key ),
		_value	( value )
	{ }

	inline K&	key		( ) { return _key;		}
	inline V&	value	( ) { return _value;	}
	inline u32	next	( ) { return _next;		}
	inline u32	prev	( ) { return _prev;		}
	
	inline void set_key		( K const& key		) { _key	= key;		}
	inline void set_value	( V const& value	) { _value	= value;	}
	inline void set_next	( u32 next			) { _next	= next;		}
	inline void set_prev	( u32 prev			) { _prev	= prev;		}
};

template<typename KVStore, typename KVStorePool>
struct hash_map_kv_store_index_16
{
	typedef u16 value_type;

	static const u16 null_value = 0xFFFF;

	static inline KVStore* get( KVStorePool& pool, value_type index )
	{
		return ( index == null_value ) ? nullptr : ( (KVStore*)pool.get_data( ) + index );
	}
	
	static inline value_type index_of( KVStorePool& pool, KVStore* kv )
	{
		return (value_type)( kv - (KVStore*)pool.get_data( ) );
	}
};

template<typename KVStore, typename KVStorePool>
struct hash_map_kv_store_index_32
{
	typedef u32 value_type;

	static const u32 null_value = 0xFFFFFFFF;

	static inline KVStore* get( KVStorePool& pool, value_type index )
	{
		return ( index == null_value ) ? nullptr : ( (KVStore*)pool.get_data( ) + index );
	}
	
	static inline value_type index_of( KVStorePool& pool, KVStore* kv )
	{
		return (value_type)( kv - (KVStore*)pool.get_data( ) );
	}
};

enum
{
	hash_map_allocator_default_pool_page_size = Memory_Page_Size,
	hash_map_allocator_default_pool_page_max_count = 256
};

template<typename K, typename V, uptr PoolPageSize = hash_map_allocator_default_pool_page_size, uptr PoolPageMaxCount = hash_map_allocator_default_pool_page_max_count>
using hash_map16 = hash_map_template<K, V,
	hash_map_hash_pred_adapter<K, hash16>,
	hash_map_key_equal_pred_adapter<K>,
	hash_map_kv_store_16<K, V>,
	hash_map_kv_store_index_16<
		hash_map_kv_store_16<K, V>,
		dynamic_pool<sizeof(hash_map_kv_store_16<K, V>), PoolPageSize, PoolPageMaxCount>
	>,
	dynamic_pool<sizeof(hash_map_kv_store_16<K, V>), PoolPageSize, PoolPageMaxCount>
>;

template<typename K, typename V, uptr PoolPageSize = hash_map_allocator_default_pool_page_size, uptr PoolPageMaxCount = hash_map_allocator_default_pool_page_max_count>
using hash_map32 = hash_map_template<K, V,
	hash_map_hash_pred_adapter<K, hash32>,
	hash_map_key_equal_pred_adapter<K>,
	hash_map_kv_store_32<K, V>,
	hash_map_kv_store_index_32<
		hash_map_kv_store_32<K, V>,
		dynamic_pool<sizeof(hash_map_kv_store_32<K, V>), PoolPageSize, PoolPageMaxCount>
	>,
	dynamic_pool<sizeof(hash_map_kv_store_32<K, V>), PoolPageSize, PoolPageMaxCount>
>;

template<typename K, typename V, uptr PoolPageSize = hash_map_allocator_default_pool_page_size, uptr PoolPageMaxCount = hash_map_allocator_default_pool_page_max_count>
using hash_map = hash_map16<K, V, PoolPageSize, PoolPageMaxCount>;

#include "hash_map_inline.h"

#endif // #ifndef __core_hash_map_h_included_