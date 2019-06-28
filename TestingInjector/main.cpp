#include <Windows.h>
#include <conio.h>
#include <iostream>

const LPSTR szPath = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\csgo.exe -insecure -window";

bool DoInject(HANDLE, const char*);

int main(int argc, char** argv)
{
	SetConsoleTitleA("RunProcess");

	char  dllname[MAX_PATH];
	GetModuleFileNameA(0, dllname, MAX_PATH);
	dllname[strlen(dllname) - 3] = 0;
	strcat_s(dllname, "dll");

	STARTUPINFOA cif;
	ZeroMemory(&cif, sizeof(STARTUPINFOA));
	PROCESS_INFORMATION pi;
	if (CreateProcessA(NULL, szPath,
		NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi) == TRUE)
	{
		std::cout << "Handle " << pi.hProcess << "ID: " << pi.dwProcessId << std::endl;
		Sleep(10000);
		if (DoInject(pi.hProcess, dllname))
		{
			std::cout << "GOOD " << std::endl;
		}
	}
	else
		std::cout << "CreateProcessA::Error " << GetLastError() << std::endl;

	Sleep(5000);
	return EXIT_SUCCESS;
}

bool DoInject(HANDLE hHandel, const char* dllPath)
{
	size_t size = strlen(dllPath) + 1;
	LPVOID loadLibAddr = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
	LPVOID pDllPath = VirtualAllocEx(hHandel, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath)
	{
		CloseHandle(hHandel);
		std::cout << "Error:Reserving memory::Code = " << GetLastError() << std::endl;
		return false;
	}
	if (!WriteProcessMemory(hHandel, pDllPath, dllPath, size, nullptr))
	{
		CloseHandle(hHandel);
		return false;
	}
	HANDLE hThread = CreateRemoteThread(hHandel, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, pDllPath, 0, 0);
	if (hThread == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error::Invalid handle""Code:: = " << GetLastError() << std::endl;
		return false;
	}
	WaitForSingleObject(hThread, INFINITE);
	if (!VirtualFreeEx(hHandel, pDllPath, NULL, MEM_RELEASE))
	{
		CloseHandle(hThread);
		std::cout << "Error::Free memory:: Code = " << GetLastError() << std::endl;
		return false;
	}
	CloseHandle(hThread);
	return true;
}
