#pragma once
#include <limits>
#include "..//singleton.hpp"

typedef unsigned long ThreadId_t;
#define FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name

//-----------------------------------------------------------------------------
//
// Simple thread creation. Differs from VCR mode/CreateThread/_beginthreadex
// in that it accepts a standard C function rather than compiler specific one.
//
//-----------------------------------------------------------------------------
FORWARD_DECLARE_HANDLE(ThreadHandle_t);
typedef unsigned (*ThreadFunc_t)(void* pParam);

//-----------------------------------------------------------------------------
// Simple thread functions. 
// Because _beginthreadex uses stdcall, we need to convert to cdecl
//-----------------------------------------------------------------------------
struct ThreadProcInfo_t
{
	ThreadProcInfo_t(ThreadFunc_t pfnThread, void* pParam)
		: pfnThread(pfnThread),
		pParam(pParam)
	{
	}

	ThreadFunc_t pfnThread;
	void* pParam;
};

ThreadHandle_t CreateSimpleThread_h(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread);
bool ReleaseThreadHandle_h(ThreadHandle_t handle);
unsigned int ThreadGetCurrentId_h();


/*class ThreadsHelper : public Singleton<ThreadsHelper>
{
public:
	ThreadHandle_t StartThread(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread);
	bool ReleaseThreadHandle(ThreadHandle_t);
};*/