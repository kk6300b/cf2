#include "232.h"
#include "SSS.h"
static
HMODULE g_OldModule = NULL, MyModule = NULL;
VOID WINAPI Free()
{
	if (g_OldModule)
	{
		FreeLibrary(g_OldModule);
	}

}

BOOL WINAPI Load()
{
	TCHAR tzPath[MAX_PATH];

	TCHAR tzTemp[MAX_PATH * 2];

	//
	// 这里是否从系统目录或当前目录加载原始DLL
	//
	//GetModuleFileName(NULL,tzPath,MAX_PATH); //获取本目录下的
	//PathRemoveFileSpec(tzPath);

	GetSystemDirectory(tzPath, MAX_PATH); //默认获取系统目录的

	lstrcat(tzPath, TEXT("\\version.dll"));

	g_OldModule = LoadLibrary(tzPath);


	if (g_OldModule == NULL)
	{
		wsprintf(tzTemp, TEXT("无法找到模块 %s,程序无法正常运行"), tzPath);
		MessageBox(NULL, tzTemp, TEXT("Error"), MB_ICONSTOP);
	}

	return (g_OldModule != NULL);

}

FARPROC WINAPI GetAddress(PCSTR pszProcName)
{
	FARPROC fpAddress;
	CHAR szProcName[64];
	TCHAR tzTemp[MAX_PATH];

	fpAddress = GetProcAddress(g_OldModule, pszProcName);
	if (fpAddress == NULL)
	{
		if (HIWORD(pszProcName) == 0)
		{
			wsprintfA(szProcName, "#%d", pszProcName);
			pszProcName = szProcName;
		}

		wsprintf(tzTemp, TEXT("无法找到函数 %hs,程序无法正常运行"), pszProcName);
		MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		ExitProcess(-2);
	}
	return fpAddress;
}

BOOL WINAPI Init()
{
	pfnAheadLib_GetFileVersionInfoA = GetAddress("GetFileVersionInfoA");
	pfnAheadLib_GetFileVersionInfoByHandle = GetAddress("GetFileVersionInfoByHandle");
	pfnAheadLib_GetFileVersionInfoExA = GetAddress("GetFileVersionInfoExA");
	pfnAheadLib_GetFileVersionInfoExW = GetAddress("GetFileVersionInfoExW");
	pfnAheadLib_GetFileVersionInfoSizeA = GetAddress("GetFileVersionInfoSizeA");
	pfnAheadLib_GetFileVersionInfoSizeExA = GetAddress("GetFileVersionInfoSizeExA");
	pfnAheadLib_GetFileVersionInfoSizeExW = GetAddress("GetFileVersionInfoSizeExW");
	pfnAheadLib_GetFileVersionInfoSizeW = GetAddress("GetFileVersionInfoSizeW");
	pfnAheadLib_GetFileVersionInfoW = GetAddress("GetFileVersionInfoW");
	pfnAheadLib_VerFindFileA = GetAddress("VerFindFileA");
	pfnAheadLib_VerFindFileW = GetAddress("VerFindFileW");
	pfnAheadLib_VerInstallFileA = GetAddress("VerInstallFileA");
	pfnAheadLib_VerInstallFileW = GetAddress("VerInstallFileW");
	pfnAheadLib_VerQueryValueA = GetAddress("VerQueryValueA");
	pfnAheadLib_VerQueryValueW = GetAddress("VerQueryValueW");
	return TRUE;
}
//HMODULE kernel32;
//HMODULE LoadLibraryW接管(LPCWSTR lpFileName)
//{
//	wstring dllName(lpFileName);
//
//	// 定义需要拦截的 DLL 名称列表
//	const vector<wstring> blockedDLLs =
//	{
//		L"ACE-CSI64.dll",
//		L"ACE-ATS64.dll",
//		L"ACE-TRACE.dll",
//		L"ACE-Tips64.dll",
//		L"ACE-Safe.dll",
//		L"ACE-Report64.dll",
//		L"ACE-IDS64.dll",
//		L"ACE-IDS.dll",
//		L"ACE-MMS64.dll",
//		L"ACE-GDP64.dll",
//		L"ACE-GDPServer64.dll",
//		L"d3dcompiler_43.dll",
//		L"D3DX9_43.dll",
//		L"d3dx11_43.dll",
//		L"ace-trace.dll",
//		L"d3d9.dll",
//		L"ACE-PBC-GAME64.dll",
//		L"ACE-DFS64.dll"
//	};
//
//	/*if (dllName.find(L"ACE-DRV64.dll") != wstring::npos)
//	{
//		printf("模块DRV替换成功\n");
//		return LoadLibraryExW(L"C:\\ACE-DRV64.dll", NULL, NULL);
//	}
//	if (dllName.find(L"ACE-DFS64.dll") != wstring::npos)
//	{
//		printf("模块DFS替换成功\n");
//		return LoadLibraryExW(L"C:\\ACE-DFS64.dll", NULL, NULL);
//	}*/
//	if (dllName.find(L"ACE-Base64.dll") != wstring::npos)
//	{
//		printf("模块Base替换成功\n");
//		return LoadLibraryExW(L"C:\\ACE-Base64.dll", NULL, NULL);
//	}
//
//
//
//
//	for (const auto& blockedDLL : blockedDLLs) {
//		if (dllName.find(blockedDLL) != wstring::npos) {
//			printf("检测模块隔离成功\n");
//			return NULL; // 返回 NULL 表示拦截
//		}
//	}
//	return LoadLibraryExW(lpFileName, NULL, NULL);
//}
//
//void 拦截模块()
//{
//
//	uintptr_t Addr = (uintptr_t)GetProcAddress(kernel32, "LoadLibraryW");
//	Addr = Addr + static_cast<unsigned long long>(读整数型(Addr + 3)) + 7;
//
//	写长整数(Addr, (uintptr_t)LoadLibraryW接管);
//}
//
//void Run()
//{
//	kernel32 = LoadLibraryA("kernel32.dll");
//	if (kernel32 == NULL) {
//		printf("模块获取失败");
//	}
//	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)拦截模块, NULL, NULL, NULL);
//}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
	HookConnect();
	HookVirtualAlloc();
	/*Run();*/
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	HELP::HideDll DLL(hModule);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		if (Load() && Init())
		{
			TCHAR szAppName[MAX_PATH] = TEXT("crossfire.exe");
			TCHAR szCurName[MAX_PATH];

			GetModuleFileName(NULL, szCurName, MAX_PATH);
			PathStripPath(szCurName);

			//是否判断宿主进程名
			if (StrCmpI(szCurName, szAppName) == 0)
			{
				DLL.RemoveLdr();//断链链表
				DLL.Removemap();//清空pe头
				//AllocConsole();
				//freopen("CONOUT$", "w", stdout);
				//system("mode con cols=50 lines=50");//窗口大小
				//SetConsoleTitleA("俄罗斯黑客大牛八个蛋网络封包器");//辅助名称
				HANDLE hThread = CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);
				if (hThread)
				{
					CloseHandle(hThread);
				}
			}
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Free();
	}

	return TRUE;
}



