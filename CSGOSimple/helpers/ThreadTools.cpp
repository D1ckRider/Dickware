#include "ThreadTools.h"
#include <Windows.h>

//ThreadHandle_t ThreadsHelper::StartThread(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread)
//{
//	using CreateSimpleThread_t = ThreadHandle_t(__cdecl*)(ThreadFunc_t, void*, size_t);
//	static auto CreateSimpleThread = (CreateSimpleThread_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "CreateSimpleThread");
//	*thread = CreateSimpleThread(start, arg, 0);
//	return *thread;
//}
//
//bool ThreadsHelper::ReleaseThreadHandle(ThreadHandle_t handle)
//{
//	using ReleaseThreadHandle_t = bool(__cdecl*)(ThreadHandle_t);
//	static auto ReleaseThreadHandle = (ReleaseThreadHandle_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "ReleaseThreadHandle");
//	return ReleaseThreadHandle(handle);
//}

ThreadHandle_t CreateSimpleThread_h(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread)
{
	using CreateSimpleThread_t = ThreadHandle_t(__cdecl*)(ThreadFunc_t, void*, size_t);
	static auto CreateSimpleThread = (CreateSimpleThread_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "CreateSimpleThread");
	*thread = CreateSimpleThread(start, arg, 0);
	return *thread;
}

bool ReleaseThreadHandle_h(ThreadHandle_t handle)
{
	using ReleaseThreadHandle_t = bool(__cdecl*)(ThreadHandle_t);
	static auto ReleaseThreadHandle = (ReleaseThreadHandle_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "ReleaseThreadHandle");
	return ReleaseThreadHandle(handle);
}

unsigned int ThreadGetCurrentId_h()
{
	using ThreadGetCurrentId_t = unsigned int(__cdecl*)();
	static auto ThreadGetCurrentId = (ThreadGetCurrentId_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "ThreadGetCurrentId");
	return ThreadGetCurrentId();
}
