#pragma once
#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
#pragma warning (disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:26495)
#pragma warning(disable:6387)
#pragma warning(disable:4005)
#pragma warning(disable:4099)
#pragma warning(disable:6031)
#pragma warning(disable:6387)
#include <windows.h>
#include<vector>
#include<Winternl.h>
#include<tlhelp32.h>
#include<string>
#include<d3d9.h>
#include <d3dx9.h>
#include<iostream>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma warning(disable:6387)
#pragma warning (disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:26495)
#pragma warning(disable:6387)
#pragma warning(disable:4005)
#pragma warning(disable:4099)
#pragma warning(disable:6031)
#pragma warning(disable:6387)
#include <Shlwapi.h>
#include "Detour/detours.h"
#pragma comment( lib, "Shlwapi.lib")
#include <atlstr.h> 
#include< stdio.h >
#include <fstream>     // 包含文件操作相关的头文件
#include <cstdio>      // 包含 remove 函数的头文件
#include "Winuser.h"
#include <tchar.h>
#include <time.h>
#include <sstream>
#include <shlobj.h>
#include <winnls.h>
#include <math.h>
#include <psapi.h>
#include <memoryapi.h>//内存指针API
#include <random>//随机数
#include "shellapi.h"//打开文件
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <map>
#include <intrin.h>
#include <codecvt>
#include <Winsock2.h>
#include <urlmon.h>
#include <direct.h>
#include <thread>

#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=AheadLib_GetFileVersionInfoA,@1")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=AheadLib_GetFileVersionInfoByHandle,@2")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExA=AheadLib_GetFileVersionInfoExA,@3")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=AheadLib_GetFileVersionInfoExW,@4")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=AheadLib_GetFileVersionInfoSizeA,@5")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExA=AheadLib_GetFileVersionInfoSizeExA,@6")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=AheadLib_GetFileVersionInfoSizeExW,@7")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=AheadLib_GetFileVersionInfoSizeW,@8")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=AheadLib_GetFileVersionInfoW,@9")
#pragma comment(linker, "/EXPORT:VerFindFileA=AheadLib_VerFindFileA,@10")
#pragma comment(linker, "/EXPORT:VerFindFileW=AheadLib_VerFindFileW,@11")
#pragma comment(linker, "/EXPORT:VerInstallFileA=AheadLib_VerInstallFileA,@12")
#pragma comment(linker, "/EXPORT:VerInstallFileW=AheadLib_VerInstallFileW,@13")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=KERNEL32.VerLanguageNameA,@14")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=KERNEL32.VerLanguageNameW,@15")
#pragma comment(linker, "/EXPORT:VerQueryValueA=AheadLib_VerQueryValueA,@16")
#pragma comment(linker, "/EXPORT:VerQueryValueW=AheadLib_VerQueryValueW,@17")
extern "C"
{
	PVOID pfnAheadLib_GetFileVersionInfoA;
	PVOID pfnAheadLib_GetFileVersionInfoByHandle;
	PVOID pfnAheadLib_GetFileVersionInfoExA;
	PVOID pfnAheadLib_GetFileVersionInfoExW;
	PVOID pfnAheadLib_GetFileVersionInfoSizeA;
	PVOID pfnAheadLib_GetFileVersionInfoSizeExA;
	PVOID pfnAheadLib_GetFileVersionInfoSizeExW;
	PVOID pfnAheadLib_GetFileVersionInfoSizeW;
	PVOID pfnAheadLib_GetFileVersionInfoW;
	PVOID pfnAheadLib_VerFindFileA;
	PVOID pfnAheadLib_VerFindFileW;
	PVOID pfnAheadLib_VerInstallFileA;
	PVOID pfnAheadLib_VerInstallFileW;
	PVOID pfnAheadLib_VerQueryValueA;
	PVOID pfnAheadLib_VerQueryValueW;



	int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
	int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
}
#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif





std::string get_current_directory()
{

	char current_directory[MAX_PATH];
	if (!_getcwd(current_directory, sizeof(current_directory))) { return  ""; }
	else
	{
		return std::string(current_directory);
	}

}




void __stdcall Free();
BOOL __stdcall Load();
FARPROC __stdcall GetAddress(PCSTR pszProcName);
BOOL __stdcall Init();
LRESULT __stdcall self_Connect(SOCKET s, const struct sockaddr* name, int namelen);
void HookConnect();
DWORD __stdcall ThreadProc(LPVOID lpThreadParameter);
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved);


BOOL ProtectVirtualMemory(HANDLE ProcessHandle, PVOID64 Addr, size_t dwSize, uintptr_t NewProtect, PULONG64 OldProtect)//修改内存初始化
{
	return reinterpret_cast<BYTE(__cdecl*)(HANDLE, OUT PVOID64*, OUT PULONG64, ULONG64, PULONG64)>
		(GetProcAddress(LoadLibrary((L"Ntdll.dll")), ("NtProtectVirtualMemory")))
		(ProcessHandle, &Addr, &dwSize, NewProtect, OldProtect) == S_OK;
}
template<typename T>
BOOL 修改内存属性(uintptr_t dwAddress, const T& Int)//修改内存属性（地址, 长度）ProtectWrite
{
	if (!IsBadReadPtr(reinterpret_cast<PVOID64>(dwAddress), sizeof(T)))
	{
		uintptr_t OldProtect = 0;
		if (ProtectVirtualMemory((HANDLE)-1, (PVOID64)dwAddress, sizeof(T), 64, &OldProtect))
		{
			*reinterpret_cast<T*>(dwAddress) = Int;
			ProtectVirtualMemory((HANDLE)-1, (PVOID64)dwAddress, sizeof(T), OldProtect, &OldProtect);
		}
		return true;
	}
	return false;
}

//清除PE头系列
namespace HELP {
	__forceinline int Rand(int Min, int Max)
	{
		int x = Min, x1 = Max;
		if (x1 < x)
		{
			Min = x1;
			Max = x;
		}
		auto CpuTime = (int)__rdtsc() * 0x343fd;
		CpuTime += 0x269ec3;
		CpuTime >>= 0x10;
		CpuTime &= 0x7fff;
		auto randnum = CpuTime % (Max - Min + 1);
		return randnum + Min;
	}

	inline DWORD GetThread()//获取线程
	{
		auto Pid = GetCurrentProcessId();
		auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, Pid);
		if (!hSnapshot)
			return NULL;

		THREADENTRY32 lpte = { sizeof(lpte) };
		if (Thread32First(hSnapshot, &lpte))
		{
			do {
				if (lpte.th32OwnerProcessID == Pid)
					return lpte.th32ThreadID;
			} while (Thread32Next(hSnapshot, &lpte));
		}
		CloseHandle(hSnapshot);
		return NULL;
	}
	class HideDll
	{
	public:
		HideDll(HMODULE hModule)
		{
			m_hModule = hModule;
		};

		DWORD64 GetModuleLen()
		{
			auto hModule = m_hModule;
			auto pImage = (PBYTE)hModule;
			PIMAGE_DOS_HEADER pImageDosHeader;
			PIMAGE_NT_HEADERS pImageNtHeader;
			pImageDosHeader = (PIMAGE_DOS_HEADER)pImage;
			if (pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
				return 0;
			pImageNtHeader = (PIMAGE_NT_HEADERS)&pImage[pImageDosHeader->e_lfanew];
			if (pImageNtHeader->Signature != IMAGE_NT_SIGNATURE)
				return 0;
			return pImageNtHeader->OptionalHeader.SizeOfImage;
		}

		BOOL Removemap()//清空pe头
		{
			auto Len = GetModuleLen();
			auto DllHandle = m_hModule;
			auto ZwUnmapViewOfSection = GetProcAddress(LoadLibrary((L"Ntdll.dll")), ("ZwUnmapViewOfSection")); if (!ZwUnmapViewOfSection)return false;
			auto ZwAllocateVirtualMemory = GetProcAddress(LoadLibrary((L"Ntdll.dll")), ("ZwAllocateVirtualMemory")); if (!ZwAllocateVirtualMemory)return false;
			auto DllLen = Len;
			auto DllAddr = new BYTE[Len];
			BYTE shellcode[256] =
			{
		0x4C, 0x89, 0x4C, 0x24, 0x20, 0x4C, 0x89, 0x44, 0x24, 0x18, 0x48, 0x89, 0x54, 0x24, 0x10, 0x48,
		0x89, 0x4C, 0x24, 0x08, 0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x8B, 0x54, 0x24, 0x58, 0x48,
		0x8B, 0x4C, 0x24, 0x50, 0xFF, 0x54, 0x24, 0x70, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x32, 0xC0, 0xEB,
		0x75, 0x48, 0xC7, 0x44, 0x24, 0x28, 0x40, 0x00, 0x00, 0x00, 0x48, 0xC7, 0x44, 0x24, 0x20, 0x00,
		0x20, 0x00, 0x00, 0x4C, 0x8D, 0x4C, 0x24, 0x60, 0x45, 0x33, 0xC0, 0x48, 0x8D, 0x54, 0x24, 0x58,
		0x48, 0x8B, 0x4C, 0x24, 0x50, 0xFF, 0x54, 0x24, 0x78, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x32, 0xC0,
		0xEB, 0x44, 0x48, 0xC7, 0x44, 0x24, 0x28, 0x40, 0x00, 0x00, 0x00, 0x48, 0xC7, 0x44, 0x24, 0x20,
		0x00, 0x10, 0x00, 0x00, 0x4C, 0x8D, 0x4C, 0x24, 0x60, 0x45, 0x33, 0xC0, 0x48, 0x8D, 0x54, 0x24,
		0x58, 0x48, 0x8B, 0x4C, 0x24, 0x50, 0xFF, 0x54, 0x24, 0x78, 0x48, 0x85, 0xC0, 0x74, 0x04, 0x32,
		0xC0, 0xEB, 0x13, 0x48, 0x8B, 0x7C, 0x24, 0x58, 0x48, 0x8B, 0x74, 0x24, 0x68, 0x48, 0x8B, 0x4C,
		0x24, 0x60, 0xF3, 0xA4, 0xB0, 0x01, 0x48, 0x83, 0xC4, 0x38, 0x5F, 0x5E, 0xC3
			};

			memcpy(DllAddr, DllHandle, Len);
			DWORD64 OldProtect = 0;
			ProtectVirtualMemory((HANDLE)-1, &shellcode, sizeof(shellcode), 64, &OldProtect);
			auto result = reinterpret_cast<BOOL(__cdecl*)(INT64, HMODULE, DWORD64, PVOID64, FARPROC, FARPROC)>(&shellcode)(-1, DllHandle, Len, DllAddr, ZwUnmapViewOfSection, ZwAllocateVirtualMemory);
			ProtectVirtualMemory((HANDLE)-1, &shellcode, sizeof(shellcode), OldProtect, &OldProtect);
			delete[] DllAddr;
			return result;
		}
		void RemovePEH()//卸载镜像
		{
			auto DllHandle = m_hModule;
			unsigned char* ImageBase = reinterpret_cast<unsigned char*>(DllHandle);
			auto DosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(ImageBase);
			if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
				return;
			auto NTHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(ImageBase + DosHeader->e_lfanew);
			if (NTHeader->Signature != IMAGE_NT_SIGNATURE)
				return;
			for (int i = 0; i < 0x1000; i++)
				ImageBase[i] = (BYTE)Rand(1, 255);
		}
		BOOL RemoveLdr()//断链链表
		{
			auto hModule = m_hModule;
			PBYTE pPeb = (PBYTE)NtCurrentTeb()->ProcessEnvironmentBlock;
			PPEB_LDR_DATA pLdr;
			PLDR_DATA_TABLE_ENTRY pLdrData;
			pLdr = (PPEB_LDR_DATA)(*(DWORD64*)(pPeb + offsetof(struct _PEB, Ldr)));
			pLdrData = (PLDR_DATA_TABLE_ENTRY)pLdr->InLoadOrderModuleList.Flink;
			do
			{
				if (pLdrData->DllBase == hModule)
				{
					pLdrData->InLoadOrderLinks.Blink->Flink = pLdrData->InLoadOrderLinks.Flink;
					pLdrData->InLoadOrderLinks.Flink->Blink = pLdrData->InLoadOrderLinks.Blink;
					pLdrData->InMemoryOrderLinks.Blink->Flink = pLdrData->InMemoryOrderLinks.Flink;
					pLdrData->InMemoryOrderLinks.Flink->Blink = pLdrData->InMemoryOrderLinks.Blink;
					pLdrData->InInitializationOrderLinks.Blink->Flink = pLdrData->InInitializationOrderLinks.Flink;
					pLdrData->InInitializationOrderLinks.Flink->Blink = pLdrData->InInitializationOrderLinks.Blink;
					return true;
				}
				pLdrData = (PLDR_DATA_TABLE_ENTRY)(pLdrData->InLoadOrderLinks.Flink);
			} while (pLdrData->DllBase);
			return false;
		}

		~HideDll() {};
	private:
		typedef struct _UNICODE_STRING
		{
			USHORT    Length;
			USHORT    MaximumLength;
			PWSTR     Buffer;

		} UNICODE_STRING, * PUNICODE_STRING;
		typedef struct _PEB_LDR_DATA
		{
			ULONG                   Length;
			BOOLEAN                 Initialized;
			PVOID                   SsHandle;
			LIST_ENTRY              InLoadOrderModuleList;
			LIST_ENTRY              InMemoryOrderModuleList;
			LIST_ENTRY              InInitializationOrderModuleList;

		} PEB_LDR_DATA, * PPEB_LDR_DATA;
		typedef struct _LDR_DATA_TABLE_ENTRY
		{
			LIST_ENTRY InLoadOrderLinks;
			LIST_ENTRY InMemoryOrderLinks;
			LIST_ENTRY InInitializationOrderLinks;
			PVOID DllBase;
			PVOID EntryPoint;
			ULONG SizeOfImage;
			UNICODE_STRING FullDllName;
			UNICODE_STRING BaseDllName;
			ULONG Flags;
			USHORT LoadCount;
			USHORT TlsIndex;
			union
			{
				LIST_ENTRY HashLinks;
				struct
				{
					PVOID SectionPointer;
					ULONG CheckSum;
				};
			};
			ULONG TimeDateStamp;

		} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;
		HMODULE m_hModule;
	};

}

struct 基址型
{
	unsigned __int64 cshell;
	__int64 障碍基址;
	__int64 红名地址;
	HWND 游戏句柄;
	__int64 模式地址;
	__int64 玩家数组;
	__int64 本人ID;
	__int64 矩阵地址;
	__int64 敌人地址;
	__int64 数组大小;
	__int64 是否游戏;
	__int64 生存偏移;
	__int64 人物地址;
	__int64 骨骼偏移;
	__int64 挂接构造地址;
	__int64 挂接地址;
	__int64 挂接中转地址;
	__int64 挂接跳回地址;
	__int64 空白地址;
	__int64 引擎文本;
	__int64 文本设备;
	__int64 绘制指针;
	__int64 线条设备;
	__int64 无后;
	__int64 无后一点1;
	__int64 无后一点2;
	__int64 鼠标X;
	__int64 鼠标Y;
	__int64 静默Hook点;
	__int64 追踪构造;
	unsigned __int64 BASE;
	unsigned __int64 tc;

};

IDirect3DDevice9* 设备地址 = NULL;
struct D3D坐标
{
	float x;
	float y;
	float z;
};
D3D坐标 本人坐标,脚轴坐标, 头轴坐标, 敌人坐标, 瞄准坐标;
struct 数据型
{
	int 人物数量;
	int 自己位置;
	int 是否敌人;
	int 人物距离;
	int 锁定pawn;
	int 上次距离;
	int 游戏中心X, 游戏中心Y, 无敌;
	int 游戏宽度,游戏高度;
};
基址型 基址;
数据型 数据;


struct 矩形数据
{
	FLOAT left;
	FLOAT top;
	FLOAT Width;
	FLOAT High;
};

矩形数据 矩形;
bool 自瞄, 龙凤连跳, 瞄准位置, 判断障碍, 瞄准热键, 是否可视, 防AI模式, 无后, 人名, 透视, 鬼跳, shift键, 骨骼, 方框1, 血量, 人名1, 显示菜单;
float 目标距离, 最近距离 = NULL;

struct 屏幕坐标
{
	__int64 x;
	__int64 y;
	__int64 w;
	__int64 h;
};

bool WriteBytesE(ULONG64 Address, std::vector<unsigned char>Data)
{
	if (IsBadReadPtr((VOID*)Address, 4))
	{
		return false;
	}
	DWORD old_protect;
	VirtualProtect((LPVOID)Address, 4, 64, &old_protect);
	for (int i = 0; i < Data.size(); i++)
	{
		*(unsigned char*)(Address)++ = Data[i];
	}
	VirtualProtect((LPVOID)Address, 4, old_protect, &old_protect);
	return true;
}
int Readx64int(__int64 Address) //读 4字节
{
	return (int)*(__int64*)(Address);
}
template<class T> T Read(DWORD64 dwPtr)
{
	if (!IsBadReadPtr((void*)dwPtr, sizeof(T)))
	{
		return *(T*)dwPtr;
	}
	return {};
}
float 读小数型(ULONG64 Address)
{
	if (IsBadReadPtr((VOID*)Address, 8))
	{
		return 0;
	}
	return *(float*)Address;
}
__int64 读长整数(__int64 Address) // 8字节
{
	if (IsBadReadPtr((VOID*)Address, 8))
	{
		return 0;
	}
	return *(__int64*)(Address);
}
int 读整数型(__int64 Address) //读 4字节
{
	if (IsBadReadPtr((VOID*)Address, 8))
	{
		return 0;
	}
	return (int)*(__int64*)(Address);
}
BYTE 读字节型(__int64 Address)
{
	return *(BYTE*)(Address);//我之前写的bug
}
char* 读文本型(__int64 address)
{
	return (char*)(address);
}
bool 写整数型(__int64 Address, int Data)
{
	DWORD old_protect;
	VirtualProtect((LPVOID)Address, 8, PAGE_EXECUTE_READWRITE, &old_protect);
	*(int*)(Address)++ = (int)Data;
	VirtualProtect((LPVOID)Address, 8, old_protect, &old_protect);
	return TRUE;
}
bool 写长整数(DWORD64 Address, DWORD64 Data)
{
	DWORD old_protect;
	VirtualProtect((LPVOID)Address, 8, PAGE_EXECUTE_READWRITE, &old_protect);
	*(__int64*)(Address)++ = Data;
	VirtualProtect((LPVOID)Address, 8, old_protect, &old_protect);
	return TRUE;
}
bool 写字节集(DWORD64 Address, std::vector<unsigned char>Data)
{
	if (IsBadReadPtr((VOID*)Address, 4))
	{
		return false;
	}
	DWORD old_protect;
	VirtualProtect((LPVOID)Address, 4, 64, &old_protect);
	for (int i = 0; i < Data.size(); i++)
	{
		*(unsigned char*)(Address)++ = Data[i];
	}
	VirtualProtect((LPVOID)Address, 4, old_protect, &old_protect);
	return true;
}
void 读字节集(__int64 Address, __int64 size, BYTE* buff)
{
	HANDLE hProcess = GetCurrentProcess();
	ReadProcessMemory(hProcess, (LPCVOID)Address, buff, size, NULL); //此处buff不需要加&符号
	CloseHandle(hProcess);
}
float 写小数型(__int64 Address, float Data)
{
	*(float*)(Address)++ = Data;
	return TRUE;
}
BYTE ReadByTes(ULONG64 参_内存地址)
{
	if (IsBadReadPtr((VOID*)参_内存地址, 1))
	{
		return 0;
	}
	return *(BYTE*)参_内存地址;
}
bool 写字节集2(__int64 Address, BYTE Data[], int Size)
{
	DWORD OldProtect;
	VirtualProtect((LPVOID)Address, Size, 64, &OldProtect);
	if (IsBadReadPtr((VOID*)Address, Size))
	{
		VirtualProtect((LPVOID)Address, Size, OldProtect, &OldProtect);
		return false;
	}
	memcpy((__int64*)Address, Data, Size);
	VirtualProtect((LPVOID)Address, Size, OldProtect, &OldProtect);
	return true;
}
BYTE ReadByte(DWORD64 参_内存地址)
{
	if (IsBadReadPtr((VOID*)参_内存地址, 1))
	{
		return 0;
	}
	return *(BYTE*)参_内存地址;
}
bool Writebyteset(DWORD64 地址, BYTE Data[], int size)
{
	DWORD old_protect = { 0 };
	VirtualProtect((LPVOID)地址, 4, PAGE_EXECUTE_READWRITE, &old_protect);
	memcpy((unsigned char*)地址, Data, size);
	VirtualProtect((LPVOID)地址, 4, old_protect, &old_protect);
	return true;
}