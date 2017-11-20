#include "mt_worker.h"
#include "../macros.h"

mt_worker::mt_worker( u32 count ) : m_threads{}
{
	ASSERT( count < 8, "threads count should be less than 8" );
	for ( u32 i = 0; i < count; ++i )
	{
		m_threads[i] = mt_worker_thread( );
	}
}

void mt_worker::destroy( )
{
	for ( u32 i = 0; i < 8; ++i )
		m_threads[i].destroy( 0 );
}

mt_worker::mt_worker_thread::mt_worker_thread( ) :
	thread( job, 0, this )
{ }

void mt_worker::mt_worker_thread::job( void* arg )
{

}