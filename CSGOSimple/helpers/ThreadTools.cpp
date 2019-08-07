#include "ThreadTools.h"
#include <Windows.h>

ThreadHandle_t CreateSimpleThread_h(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread)
{
	using CreateSimpleThread_t = ThreadHandle_t(__cdecl*)(ThreadFunc_t, void*, size_t);
	static auto CreateSimpleThread = (CreateSimpleThread_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "CreateSimpleThread");
	*thread = CreateSimpleThread(start, arg, 0);
	return *thread;
}
