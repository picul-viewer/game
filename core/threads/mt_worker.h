#ifndef __MT_WORKER_H_
#define __MT_WORKER_H_

#include <Windows.h>
#include "types.h"
#include "thread.h"

class mt_worker
{
public:
	mt_worker( u32 count );
	~mt_worker( ) = default;

	void destroy( );

protected:
	class mt_worker_thread : public thread
	{
	public:
		mt_worker_thread( );

	private:
		static void job( void* arg );
	};

	mt_worker_thread m_threads[8];
};

#endif // #ifndef __MT_WORKER_H_