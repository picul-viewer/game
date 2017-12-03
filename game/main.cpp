#include <core/game.h>

#define OPCOUNT 100'000'000

static u32 g = 0;

struct Obj
{
	aabb_aligned aabb;

	Obj* prev;
	Obj* next;

	pointer node;

	aabb_aligned const& get_aabb( ) const { return aabb; }
};

using node_container = embedded_twosided_list<Obj, &Obj::prev, &Obj::next>;

using node_heap = array_heap<octree_node_size<node_container>::value, 1024, aligned_mem_allocator<64>>;

octree<Obj, node_container, node_heap, &Obj::node> tree;

void callback(Obj*)
{
	printf("Hello!\n");
}

int main( )
{
	mem_align(16)
	aabb a[3];
	mem_align(16)
	aabb_aligned b[3];

	mem_align(64)
	math::float4x4 matrix = math::matrix_perspective( 65.0f * math::c_radians, 1.3333f, 1.0f, 100.0f );

	mem_align(16)
	math::float4 aabb_min0( -10.0f, -20.0f, -30.0f, 1.0f );
	mem_align(16)
	math::float4 aabb_max0( 30.0f, 20.0f, 10.0f, 1.0f );
	mem_align(16)
	math::float4 aabb_min1( -1.0f, -1.0f, -1.0f, 1.0f );
	mem_align(16)
	math::float4 aabb_max1( 1.0f, 1.0f, 1.0f, 1.0f );
	mem_align(16)
	math::float4 aabb_min2( -1.0f, -2.0f, -3.0f, 1.0f );
	mem_align(16)
	math::float4 aabb_max2( -0.5f, -1.0f, -1.0f, 1.0f );

	a[0].set_min_max( aabb_min0, aabb_max0 );
	b[0].set_min_max( aabb_min0, aabb_max0 );
	a[1].set_min_max( aabb_min1, aabb_max1 );
	b[1].set_min_max( aabb_min1, aabb_max1 );
	a[2].set_min_max( aabb_min2, aabb_max2 );
	b[2].set_min_max( aabb_min2, aabb_max2 );
	
	frustum f0;
	frustum_aligned f1;

	f0.set_from_matrix( matrix );
	f1.set_from_matrix( matrix );

	node_heap h;
	tree.create( h, b[0], 1.0f );
	Obj* o = new Obj( );
	o->aabb = b[1];
	tree.insert( o );
	tree.remove( o );
	
	tree.query_visibility( f1, &callback );

	timer t;
	u64 time;
	
	t.start( );
	for ( u32 i = 0; i < OPCOUNT; ++i )
		g += i % 3;
	u64 def_time = t.get_elapsed_ms( );
	
	t.start( );
	for ( u32 i = 0; i < OPCOUNT; ++i )
		g += f0.test_aabb( a[i % 3] );
	time = t.get_elapsed_ms( );
	printf( "Default: %lldms\n", time - def_time );
	
	t.start( );
	for ( u32 i = 0; i < OPCOUNT; ++i )
		g += f1.test_aabb( b[i % 3] );
	time = t.get_elapsed_ms( );
	printf( "Aligned: %lldms\n", time - def_time );

	system( "pause" );
}