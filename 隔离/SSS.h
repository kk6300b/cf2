#pragma once

#include <WinSock2.h>
#include <stdio.h>
#include <time.h>
#include<Winternl.h>
#include <TlHelp32.h>
#include <process.h> 
#include <Psapi.h>
#include <iostream>
#include<Windows.h>   
#include <vector> 
#include <thread> 
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <atlstr.h>  
#include "Detour/detours.h"
#include <chrono>
#include <ctime>
#include <mutex>
#include <condition_variable>
#include "数据.h"
#include "232.h"
#include "winerror.h"
#pragma comment(lib,"ws2_32.lib")
IDirect3DStateBlock9* pStateBlock = nullptr;
ID3DXFont* g_FontSmall = nullptr;
ID3DXLine* m_ID3DXLine = nullptr;
typedef int (WINAPI* typeHookSend)(SOCKET s, const char* buf, int len, int flag);
typeHookSend pOldSend = nullptr;

typedef int (WINAPI* typeConnect)(
    _In_ SOCKET s,
    _In_reads_bytes_(namelen) const struct sockaddr FAR* name,
    _In_ int namelen);
typeConnect pOldConnect = nullptr;

typedef int (WINAPI* typeHookRecv)(SOCKET s, const char* buf, int len, int flag);
typeHookRecv pOldRecv = nullptr;

ULONG64 开关地址, X地址, Y地址, 跳回地址;
using namespace std;
#pragma warning(disable:4996)
bool OpenRegistryKey(HKEY rootKey, const std::wstring& subKey, HKEY& hKey) {
    LONG result = RegOpenKeyEx(rootKey, subKey.c_str(), 0, KEY_READ, &hKey);
    return result == ERROR_SUCCESS;
}
bool QueryRegistryValue(HKEY hKey, const std::wstring& valueName) {
    DWORD dwType;
    BYTE buffer[512];
    DWORD dwBufferSize = sizeof(buffer);

    LONG result = RegQueryValueEx(hKey, valueName.c_str(), nullptr, &dwType, buffer, &dwBufferSize);
    return result == ERROR_SUCCESS;
}
void CloseRegistryKey(HKEY hKey) {
    RegCloseKey(hKey);
}
bool CheckRegistryValue(const std::wstring& regPath, const std::wstring& valueName) {
    HKEY hKey = nullptr;

    // 尝试打开注册表键
    if (!OpenRegistryKey(HKEY_LOCAL_MACHINE, regPath, hKey)) {
        std::wcerr << L"无法打开注册表路径: " << regPath << std::endl;
        return false;
    }

    // 查询指定的注册表值
    bool valueExists = QueryRegistryValue(hKey, valueName);

    // 关闭注册表键
    CloseRegistryKey(hKey);

    return valueExists;
}
bool isFileExists_fopen(LPCSTR name) {
    if (FILE* file = fopen(name, ("r")))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}
void DeleteSubKeys(HKEY hKey, const char* subKey) {
    // 打开指定的注册表项
    HKEY hSubKey;
    LONG result = RegOpenKeyExA(hKey, subKey, 0, KEY_READ | KEY_WRITE, &hSubKey);
    if (result != ERROR_SUCCESS) {
        std::cerr << "无法打开注册表项: " << subKey << std::endl;
        return;  // 无法打开该注册表项
    }

    // 获取子项的数量
    char subKeyName[255];
    DWORD subKeySize = sizeof(subKeyName);
    DWORD index = 0;

    while (RegEnumKeyExA(hSubKey, index++, subKeyName, &subKeySize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
        // 递归删除子项
        DeleteSubKeys(hSubKey, subKeyName);
        // 删除子项
        if (RegDeleteKeyA(hSubKey, subKeyName) != ERROR_SUCCESS) {
            std::cerr << "删除子项失败: " << subKeyName << std::endl;
        }
        subKeySize = sizeof(subKeyName);  // 重置子键名大小
    }

    RegCloseKey(hSubKey);
}
bool DeleteRegistryKey(HKEY hKey, const char* subKey) {
    // 递归删除子项
    DeleteSubKeys(hKey, subKey);

    // 删除注册表项
    LONG result = RegDeleteKeyA(hKey, subKey);
    if (result == ERROR_SUCCESS) {
        std::cout << "注册表项及其子项删除成功。" << std::endl;
        return true;
    }
    else {
        std::cerr << "删除注册表项失败，错误代码: " << result << std::endl;
        return false;
    }
}
std::string GetTextLeft(std::string Text, int Len) {

    if (Text.length() >= Len) {
        return Text.substr(0, Len);
    }
    return "";
}
std::string GetTextSub(std::string Text, int Index, int size) {
    if ((int)Text.length() >= Index + size) {
        return Text.substr(Index - 1, size);
    }
    return "";
}
std::string replaceSubstring(std::string& originalText, int start, int end, const std::string& replacement)
{
    //这里判断取的位置 不合理则返回原始文本
    if (start >= end || start < 0 || end > originalText.length())
        return originalText;

    std::string before = originalText.substr(0, start);
    std::string after = originalText.substr(end);

    return before + replacement + after;
}
int FindTextAA(std::string Text, std::string Substr) {
    size_t found = Text.find(Substr);
    if (found != std::string::npos) {
        // 子字符串在文本中找到，返回位置（位数）
        return static_cast<int>(found);
    }
    else {
        // 子字符串未找到，返回-1
        return -1;
    }
}
bool hexToBytes(const std::string& hex, unsigned char* outData, size_t& outLength) {
    if (hex.length() % 2 != 0) {
        std::cerr << "Hex string must have an even length." << std::endl;
        return false;
    }

    outLength = hex.length() / 2;
    for (size_t i = 0; i < outLength; ++i) {
        outData[i] = (unsigned char)(std::stoi(hex.substr(i * 2, 2), nullptr, 16));
    }
    return true;
}
std::string nor(const char* aok, const int Size) {
    std::stringstream stream;
    stream << std::hex << std::uppercase << std::setfill('0');
    for (int i = 0; i < Size; ++i) {
        stream << std::setw(2) << (static_cast<uint32_t>(aok[i]) & 0xff);
    }
    return stream.str();
}


int __stdcall self_Send(SOCKET s, const char* buffer, int len, int flag)
{
    std::string hexString = nor(buffer, len);
    if (len > 17) 
    {
       
        if ((BYTE)buffer[0] == 0x01) {
            printf("拦截01成功\n");
            return -1;
        }
        if ((BYTE)buffer[0] == 0xF1 && (BYTE)buffer[4] == 0xEC) {
            printf("拦截EC成功\n");
            return -1;
        }
        if ((BYTE)buffer[0] == 0xF1)
        {
            printf("\n");
            for (int i = 0; i < len; i++)
            {
                printf("%02X", (BYTE)buffer[i]);
            }
        }
        if ((BYTE)buffer[0] == 0x01)
        {
            printf("\n");
            for (int i = 0; i < len; i++)
            {
                printf("%02X", (BYTE)buffer[i]);
            }
        }
    }
    return pOldSend(s, buffer, len, flag);
}


__int64 Readx64Long(__int64 Address)
{
    if (IsBadReadPtr((VOID*)Address, 4))
    {
        return 0;
    }
    return *(ULONG64*)(Address);
}
DWORD64 取模块入口函数(LPCSTR dll, LPCSTR func) {
    DWORD64 addr = 0;
    HMODULE hModule = LoadLibraryA(dll);
    addr = (DWORD64)GetProcAddress(hModule, func);
    return addr;
}
bool WriteVecBytes(__int64 Address, BYTE Data)
{
    DWORD old_protect;
    VirtualProtect((LPVOID)Address, 1, 64, &old_protect);//修改内存属性
    *(BYTE*)Address++ = Data;
    VirtualProtect((LPVOID)Address, 1, old_protect, &old_protect);//还原内存属性
    return TRUE;

}
void 线条直线(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, D3DCOLOR Color, FLOAT Width)
{
    D3DXVECTOR2 Vertex[2] = { { X1, Y1 },{ X2, Y2 } };
    m_ID3DXLine->SetWidth(Width);
    m_ID3DXLine->Draw(Vertex, 2, Color);
}

void 线条矩形(矩形数据 Rectangle, D3DCOLOR Color, FLOAT Width)
{
    线条直线(Rectangle.left, Rectangle.top, Rectangle.left, Rectangle.top + Rectangle.High, Color, Width);
    线条直线(Rectangle.left, Rectangle.top + Rectangle.High, Rectangle.left + Rectangle.Width, Rectangle.top + Rectangle.High, Color, Width);
    线条直线(Rectangle.left, Rectangle.top, Rectangle.left + Rectangle.Width, Rectangle.top, Color, Width);
    线条直线(Rectangle.left + Rectangle.Width, Rectangle.top, Rectangle.left + Rectangle.Width, Rectangle.top + Rectangle.High, Color, Width);
}

void 绘制骨骼(int Addr, int color, int 线宽)
{
    D3D坐标 头部, 脖子, 左臂, 左肘, 左手, 右臂, 右肘, 右手, 盆骨, 左腿, 左膝, 左脚, 右腿, 右膝, 右脚;
    D3D坐标 头部O, 脖子O, 左臂O, 左肘O, 左手O, 右臂O, 右肘O, 右手O, 盆骨O, 左腿O, 左膝O, 左脚O, 右腿O, 右膝O, 右脚O;
    if (取骨骼坐标(Addr, &盆骨, 3))
    {
        取骨骼坐标(Addr, &脖子, 5);
        脖子.z += 2;
        取骨骼坐标(Addr, &头部, 6);
        头部.z += 10;
        取骨骼坐标(Addr, &左臂, 8);
        取骨骼坐标(Addr, &左肘, 9);
        取骨骼坐标(Addr, &左手, 10);
        取骨骼坐标(Addr, &右臂, 15);
        取骨骼坐标(Addr, &右肘, 16);
        取骨骼坐标(Addr, &右手, 17);
        取骨骼坐标(Addr, &左腿, 21);
        取骨骼坐标(Addr, &左膝, 22);
        取骨骼坐标(Addr, &左脚, 23);
        取骨骼坐标(Addr, &右腿, 25);
        取骨骼坐标(Addr, &右膝, 26);
        取骨骼坐标(Addr, &右脚, 27);
    }
    if (WorldToScreen(头部, &头部O))
    {
        if (WorldToScreen(脖子, &脖子O))
        {
            if (WorldToScreen(盆骨, &盆骨O))
            {
                if (WorldToScreen(左臂, &左臂O))
                {
                    if (WorldToScreen(左肘, &左肘O))
                    {
                        if (WorldToScreen(左手, &左手O))
                        {
                            if (WorldToScreen(右臂, &右臂O))
                            {
                                if (WorldToScreen(右肘, &右肘O))
                                {
                                    if (WorldToScreen(右手, &右手O))
                                    {
                                        if (WorldToScreen(左腿, &左腿O))
                                        {
                                            if (WorldToScreen(左膝, &左膝O))
                                            {
                                                if (WorldToScreen(左脚, &左脚O))
                                                {
                                                    if (WorldToScreen(右腿, &右腿O))
                                                    {
                                                        if (WorldToScreen(右膝, &右膝O))
                                                        {
                                                            if (WorldToScreen(右脚, &右脚O))
                                                            {
                                                                线条直线(盆骨O.x, 盆骨O.y, 头部O.x, 头部O.y, color, 线宽);
                                                                线条直线(脖子O.x, 脖子O.y, 左臂O.x, 左臂O.y, color, 线宽);
                                                                线条直线(左臂O.x, 左臂O.y, 左肘O.x, 左肘O.y, color, 线宽);
                                                                线条直线(左肘O.x, 左肘O.y, 左手O.x, 左手O.y, color, 线宽);
                                                                线条直线(脖子O.x, 脖子O.y, 右臂O.x, 右臂O.y, color, 线宽);
                                                                线条直线(右臂O.x, 右臂O.y, 右肘O.x, 右肘O.y, color, 线宽);
                                                                线条直线(右肘O.x, 右肘O.y, 右手O.x, 右手O.y, color, 线宽);
                                                                线条直线(盆骨O.x, 盆骨O.y, 左腿O.x, 左腿O.y, color, 线宽);
                                                                线条直线(左腿O.x, 左腿O.y, 左膝O.x, 左膝O.y, color, 线宽);
                                                                线条直线(左膝O.x, 左膝O.y, 左脚O.x, 左脚O.y, color, 线宽);
                                                                线条直线(盆骨O.x, 盆骨O.y, 右腿O.x, 右腿O.y, color, 线宽);
                                                                线条直线(右腿O.x, 右腿O.y, 右膝O.x, 右膝O.y, color, 线宽);
                                                                线条直线(右膝O.x, 右膝O.y, 右脚O.x, 右脚O.y, color, 线宽);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void 静默部署(D3D坐标 瞄准坐标)
{
    写小数型(Y地址, 瞄准坐标.y);
    写小数型(X地址, 瞄准坐标.x);
}

void 绘制填充(int 左边, int 顶边, int 宽度, int 高度, DWORD color)
{
    左边 = 左边 + 宽度 / 2;
    线条直线(左边, 顶边, 左边, 顶边 + 高度, color, 宽度);
}
ID3DXFont* g_pFont = nullptr;
const int FontSize = 15; // 默认字体大小
void CreatFont(IDirect3DDevice9* direct3ddevice9)
{
    if (FAILED(D3DXCreateFont(direct3ddevice9, FontSize, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PALETTE | FF_DONTCARE, L"宋体", &g_FontSmall)))
    {
        MessageBox(NULL, L"字体创建失败，请重试", L"温馨提示", 0);
        exit(0);
    }
    direct3ddevice9->CreateStateBlock(D3DSBT_ALL, &pStateBlock); // Stateblock saves all the rendersettings, as we are going to modify them in order to draw our stuff
    direct3ddevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    direct3ddevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    direct3ddevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    direct3ddevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    direct3ddevice9->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    direct3ddevice9->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
}
void DrawString(const char* words, int x, int y, D3DCOLOR color)
{
    RECT FontPos{};
    FontPos.left = x;
    FontPos.top = y;
    FontPos.right = 30;
    FontPos.bottom = 30;
    g_FontSmall->DrawTextA(NULL, words, -1, &FontPos, DT_NOCLIP, color);
}
float 瞄准距离, 上次距离;

void 挂接中转(ULONG64 rcx)
{
    if (设备地址 != (LPDIRECT3DDEVICE9)读长整数(基址.线条设备))
    {
        if (设备地址)
        {
            m_ID3DXLine->Release();
        }
        设备地址 = (LPDIRECT3DDEVICE9)读长整数(基址.线条设备);
        D3DXCreateLine(设备地址, &m_ID3DXLine);
        CreatFont(设备地址);
    }

    if (GetAsyncKeyState(VK_HOME) & 1)
    {
        显示菜单 = !显示菜单;
    }
    static int 菜单选中 = 2;
    if (显示菜单)
    {
        int 当前位置 = 0;
        int 左边 = 20;
        int 顶边 = 200;
        int 行距 = 20;
        DrawString((char*)"Homo键显示||隐藏菜单", 20, 160, 0xFFFFFFFF);//就这样
        DrawString((char*)"风花雪月", 65, 190, 0xFFFFFFFF);//就这样
        DrawString((char*)"√ ", 115, 菜单选中 * 行距 + 顶边, 0xFFFF0000);
        当前位置 += 1;
        DrawString(自瞄 == true ? (char*)"☆追踪开关☆   [开启]" : (char*)"☆追踪开关☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(瞄准热键 == true ? (char*)"☆追踪热键☆   [双键]" : (char*)"☆瞄准热键☆   [右键]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(瞄准位置 == true ? (char*)"☆追踪位置☆   [头部]" : (char*)"☆瞄准位置☆   [胸部]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(方框1 == true ? (char*)"☆绘制方框☆   [开启]" : (char*)"☆绘制方框☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(骨骼 == true ? (char*)"☆绘制骨骼☆   [开启]" : (char*)"☆绘制骨骼☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(人名1 == true ? (char*)"☆绘制人名☆   [开启]" : (char*)"☆绘制人名☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(血量 == true ? (char*)"☆绘制血量☆   [开启]" : (char*)"☆绘制血量☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(鬼跳 == true ? (char*)"☆龙凤鬼跳☆   [开启]" : (char*)"☆龙凤鬼跳☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(shift键 == true ? (char*)"☆静步锁头☆   [开启]" : (char*)"☆静步键头☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        {
            const char* fmt = "☆追踪范围☆    [%d]";
            char buf[4096];
            ZeroMemory(&buf, sizeof(buf));
            snprintf(buf, sizeof(buf), fmt, 自瞄范围);
            DrawString(buf, 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        }
        当前位置 += 1;
        DrawString(判断障碍 == true ? (char*)"☆障碍漏打☆   [开启]" : (char*)"☆障碍漏打☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 1;
        DrawString(无后 == true ? (char*)"☆一点无后☆   [开启]" : (char*)"☆一点无后☆   [关闭]", 左边, 当前位置 * 行距 + 顶边, 菜单选中 == 当前位置 ? 0xFFFF0000 : 0xFF00FA9A);
        当前位置 += 2;
        DrawString((char*)"上下方向鍵选择项目/左右方向鍵调节参数", 左边, 当前位置 * 行距 + 顶边, 0xFF00FA9A);
        当前位置 += 1;
        DrawString((char*)"排位模式请控制心态/請勿短时间连续击杀!!!!", 左边, 当前位置 * 行距 + 顶边, 0xFF00FA9A);

    }
    if (GetAsyncKeyState(VK_UP) & 1)
    {
        菜单选中--;
        if (菜单选中 < 1)
        {
            菜单选中 = 12;
        }
    }
    if (GetAsyncKeyState(VK_DOWN) & 1)
    {
        菜单选中++;
        if (菜单选中 > 12)
        {
            菜单选中 = 1;
        }
    }

    if (GetAsyncKeyState(37) & 1 || GetAsyncKeyState(39) & 1)
    {
        if (菜单选中 == 1)
        {
            自瞄 = !自瞄;
        }
        if (菜单选中 == 2)
        {
            瞄准热键 = !瞄准热键;
        }
        if (菜单选中 == 3)
        {
            瞄准位置 = !瞄准位置;
        }
        if (菜单选中 == 4)
        {
            方框1 = !方框1;
        }
        if (菜单选中 == 5)
        {
            骨骼 = !骨骼;
        }
        if (菜单选中 == 6)
        {
            人名1 = !人名1;
        }
        if (菜单选中 == 7)
        {
            血量 = !血量;
        }
        if (菜单选中 == 8)
        {
            鬼跳 = !鬼跳;
        }
        if (菜单选中 == 9)
        {
            shift键 = !shift键;
        }
        if (菜单选中 == 11)
        {
            判断障碍 = !判断障碍;
        }
        if (菜单选中 == 12)
        {
            无后 = !无后;
            if (无后)
            {
                写字节集(基址.无后, { 0xC3,0x90 });
                写字节集(基址.无后一点1, { 0xE9,0x1E,0x03,0x00,0x00 });
                写字节集(基址.无后一点2, { 0xE9,0xA0 ,0x02,0x00,0x00 });
            }
            else
            {
                写字节集(基址.无后, { 0x40,0x55,0x53,0x57,0x48 });
                写字节集(基址.无后一点1, { 0x0F,0x82,0x1D,0x03,0x00,0x00 });
                写字节集(基址.无后一点2, { 0x0F,0x82,0x9F,0x02,0x00,0x00 });
            }
        }
    }

    if (菜单选中 == 10)
    {
        if (GetAsyncKeyState(37) != 0)
        {
            Sleep(50);
            if (自瞄范围 == 10)
            {
                自瞄范围 = 150;
            }
            else if (自瞄范围 == 30)
            {
                自瞄范围 = 10;
            }
            else if (自瞄范围 == 60)
            {
                自瞄范围 = 30;
            }
            else if (自瞄范围 == 100)
            {
                自瞄范围 = 60;
            }
            else if (自瞄范围 == 150)
            {
                自瞄范围 = 100;
            }
        }
        else if (GetAsyncKeyState(39) != 0)
        {
            Sleep(50);
            if (自瞄范围 == 10)
            {
                自瞄范围 = 30;
            }
            else if (自瞄范围 == 30)
            {
                自瞄范围 = 60;
            }
            else if (自瞄范围 == 60)
            {
                自瞄范围 = 100;
            }
            else if (自瞄范围 == 100)
            {
                自瞄范围 = 150;
            }
            else if (自瞄范围 == 150)
            {
                自瞄范围 = 10;
            }
        }
    }
    
    if (鬼跳)
    {
        写整数型(基址.cshell + 0x46637C8, 0);
    }

    数据.游戏宽度 = 读整数型(基址.矩阵地址 + 0x08);
    数据.游戏高度 = 读整数型(基址.矩阵地址 + 0x0C);
    屏幕坐标 画出坐标[31] = { NULL };
    数据.人物数量 = 取人物数量();
    数据.自己位置 = 取自己位置();
    数据.是否敌人 = (数据.自己位置 <= 8) ? 0 : 1;
    取本人坐标(&本人坐标);
    bool 敌人 = false;
    for (size_t n = 1; n <= 31; n++)
    {
        if (数据.人物数量 == 8)
        {

            if (数据.是否敌人 == 1 ? (n + 1) - 数据.是否敌人 <= 8 : (n + 1) - 数据.是否敌人 > 9)
            {
                敌人 = true;
            }
            else
            {
                敌人 = false;
            }
        }
        if (数据.人物数量 >= 16)
        {
            敌人 = true;
        }
        if (数据.人物数量 == 30)
        {
            __int64 判断生化 = 取人物血量(n);
            __int64 自己血量 = 取人物血量(数据.自己位置);
            if (自己血量 != 0)
            {
                if (自己血量 < 101)
                {
                    if (判断生化 > 100)
                    {
                        敌人 = true;

                    }
                    else
                    {
                        敌人 = false;

                    }
                }
                else
                {
                    if (判断生化 < 101)
                    {
                        敌人 = true;

                    }
                    else
                    {
                        敌人 = false;

                    }
                }
            }
        }
        if (取人物血量(n) > 0)
        {
            if (敌人)
            {
                if (n != 数据.自己位置)
                {
                    if (取骨骼坐标(n, &敌人坐标, 0))
                    {
                        敌人坐标.z = 敌人坐标.z - 160;

                        if (WorldToScreen(敌人坐标, &脚轴坐标))
                        {
                            if (取骨骼坐标(n, &敌人坐标, 6))
                            {
                                敌人坐标.z = 敌人坐标.z + 40;
                                if (WorldToScreen(敌人坐标, &瞄准坐标))
                                {
                                    矩形.High = 脚轴坐标.y - 瞄准坐标.y;
                                    矩形.Width = 矩形.High / 2;
                                    矩形.left = 瞄准坐标.x - 矩形.High / 4;
                                    矩形.top = 瞄准坐标.y;
                                    画出坐标[n] = 转屏幕方框(n);
                                    if (自瞄)
                                    {
                                        锁定范围(瞄准坐标, n);
                                    }
                                    if (是否炸弹(n))
                                    {
                                        if (方框1)
                                        {

                                            线条矩形(矩形, 0xFFFF0000, 1);
                                        }
                                        if (骨骼)
                                        {

                                            绘制骨骼(n, 0xFFFF0000, 1);
                                        }
                                        if (血量)
                                        {

                                            char temp_char[15];
                                            sprintf(temp_char, "HP：%d", 取人物血量(n));
                                            DrawString(temp_char, 脚轴坐标.x - 20, 脚轴坐标.y + 5, 0xFFFF0000);
                                        }
                                        if (人名1)
                                        {

                                            DrawString(取敌人昵称(n), 瞄准坐标.x - 20, 瞄准坐标.y - 16, 0xFFFF0000);
                                        }
                                    }
                                    else
                                    {

                                        if (方框1)
                                        {
                                            线条矩形(矩形, 数据.锁定pawn == n ? 0xFFFF0000 : 0xFFFFFFF0, 1);
                                        }
                                        if (骨骼)
                                        {
                                            绘制骨骼(n, 数据.锁定pawn == n ? 0xFFFF0000 : 0xFFFFFFF0, 1);
                                        }
                                        if (血量)
                                        {
                                            char temp_char[15];
                                            sprintf(temp_char, "HP：%d", 取人物血量(n));
                                            DrawString(temp_char, 脚轴坐标.x - 20, 脚轴坐标.y + 5, 0xFF00FA9A);
                                        }
                                        if (人名1)
                                        {

                                            DrawString(取敌人昵称(n), 瞄准坐标.x - 20, 瞄准坐标.y - 16, 0xFF00FA9A);
                                        }


                                    }


                                }
                            }
                        }
                    }
                }
            }
        }

    }
    if (数据.锁定pawn != 0)
    {
        if (取人物血量(数据.锁定pawn))
        {
            bool shouldAim = false;

            // 双键瞄准模式
            if (瞄准热键)
            {
                shouldAim = (GetAsyncKeyState(1) != 0 || GetAsyncKeyState(2) != 0);

                // Shift键特殊处理
                if (shift键 && shouldAim)
                {
                    shouldAim = (GetAsyncKeyState(16) != 0);
                }
            }
            // 右键瞄准模式
            else
            {
                shouldAim = (GetAsyncKeyState(2) != 0);
            }

            if (自瞄 && shouldAim)
            {
                bool hitDetected = false;
                // 开启障碍检测时的优化逻辑
                if (判断障碍)
                {
                    // 定义28个骨骼点的优先级顺序（头部>身体>四肢）
                    const int BONE_PRIORITY[28] = {
                        4,  // 胸部 - 最高优先级                            
                        3, 5, 6,8,  // 脊柱和头，肩膀
                        11, 17, 12, 18,  // 上臂和前臂
                        13, 19,  // 手掌
                        2, 1, 7, 14, 15, 16,  // 躯干中部
                        9, 20, 21, 22,  // 大腿
                        10, 23, 24, 25,  // 小腿
                        26, 27, 0  // 脚部 - 最低优先级
                    };

                    // 按优先级顺序检测骨骼点可见性
                    for (int i = 0; i < 28; i++)
                    {
                        int boneIndex = BONE_PRIORITY[i];

                        // 尝试获取骨骼点坐标并检测是否可见
                        if (取骨骼坐标(数据.锁定pawn, &瞄准坐标, boneIndex) &&
                            障碍Call(本人坐标, 瞄准坐标))
                        {
                            // 调试信息 - 显示命中骨骼点
                            char boneInfo[50];
                            sprintf(boneInfo, "命中骨骼点: %d", boneIndex);
                            DrawString(boneInfo, 数据.游戏宽度 / 2 - 30, 数据.游戏高度 / 2 + 120, 0xFF00FFFF);

                            写整数型(开关地址, 1);
                            静默部署(VectorToRotation(本人坐标, 瞄准坐标));
                            hitDetected = true;
                            break;
                        }
                    }
                }
                // 未开启障碍检测时使用简单模式
                else
                {
                    // 根据设置选择瞄准头部或身体
                    int bonePoint = 瞄准位置 ? 6 : 4;

                    if (取骨骼坐标(数据.锁定pawn, &瞄准坐标, bonePoint) &&
                        障碍Call(本人坐标, 瞄准坐标))
                    {
                        写整数型(开关地址, 1);
                        静默部署(VectorToRotation(本人坐标, 瞄准坐标));
                        hitDetected = true;
                    }
                }

                // 如果没有检测到有效的瞄准点，则关闭瞄准
                if (!hitDetected)
                {
                    写整数型(开关地址, 0);
                }
            }
            else
            {
                写整数型(开关地址, 0);
                数据.锁定pawn = 0;
            }
        }
        else
        {
            写整数型(开关地址, 0);
            数据.锁定pawn = 0;
        }
    }
    
    ((void(CALLBACK*)(ULONG64)) 基址.挂接跳回地址)(rcx);
}

void 追踪构造()
{
    ULONG64 Hook原值 = 读长整数(基址.静默Hook点);
    开关地址 = 基址.追踪构造 + 58;
    X地址 = 基址.追踪构造 + 66;
    Y地址 = 基址.追踪构造 + 74;
    跳回地址 = 基址.追踪构造 + 82;
    写长整数(跳回地址, Hook原值);
    写字节集(基址.追踪构造, { 80, 131, 61, 50, 0, 0, 0, 1, 117, 33 ,72, 161, 0, 0, 0, 0, 0, 0, 0, 0, 72, 57, 68, 36, 40, 117, 16, 243, 15, 16, 29, 31, 0, 0, 0, 243, 15, 16, 21, 31, 0, 0, 0, 88, 255, 37, 32, 0, 0, 0 });
    写长整数(基址.追踪构造 + 12, 基址.人物地址);
    写长整数(基址.静默Hook点, 基址.追踪构造);
}

void 基地初始()
{
    while (!基址.cshell)
    {
        基址.cshell = (unsigned __int64)GetModuleHandleA("cshell_x64.dll");
        基址.BASE = (unsigned __int64)GetModuleHandleA("ACE-Base64.dll");
        基址.tc = (unsigned __int64)GetModuleHandleA("rail_api64.dll");
    }
    基址.追踪构造 = (DWORD64)VirtualAlloc(NULL, 4096, 4096, 64);
    基址.本人ID = 0x5CC;
    基址.骨骼偏移 = 0x29B8;
    基址.生存偏移 = 0x188;
    基址.矩阵地址 = 0x1405EC280;
    基址.障碍基址 = 0x140055B00;
    基址.玩家数组 = 基址.cshell + 0x4295E10;
    基址.模式地址 = 基址.cshell + 0x4295E6C;//判断机制
    基址.人物地址 = 读长整数(基址.玩家数组) + 0x178;
    基址.线条设备 = 0x140632B18;
    基址.静默Hook点 = 0x1404C8AB8;
    追踪构造();
    显示菜单 = false;
    自瞄 = false;
    判断障碍 = false;
    瞄准位置 = false;
    瞄准热键 = false;
    方框1 = false;
    人名1 = false;
    骨骼 = false;
    血量 = false;
    自瞄范围 = 30;
    基址.挂接跳回地址 = 读长整数(0x1405B08B8);
    写长整数(0x1405B08B8, (DWORD64)挂接中转);
    /*写字节集(基址.BASE+ 0x9677D0,{0xC3});
    写字节集(基址.BASE + 0x9677D1, { 0xC3 });*/
}


// 获取模块基址和大小
BOOL GetModuleBounds(HMODULE hModule, LPVOID* base, LPVOID* end) {
    MODULEINFO moduleInfo;
    if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
        *base = moduleInfo.lpBaseOfDll;
        *end = (BYTE*)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage;
        return TRUE;
    }
    return FALSE;
}

// 终止指定模块的所有线程
void TerminateSpecificModuleThreads() {
    // 定义要终止线程的模块列表
    const char* targetModules[] = {
        /*"crossfirebase.dll",*/
        "rail_api64.dll",
        /*"ACE-Base64.dll",*/
        "ACE-PBC-GAME64.dll"
    };

    // 存储每个模块的地址范围
    std::vector<std::pair<LPVOID, LPVOID>> moduleRanges;

    // 获取每个模块的地址范围
    for (const auto& moduleName : targetModules) {
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (hModule) {
            LPVOID base, end;
            if (GetModuleBounds(hModule, &base, &end)) {
                moduleRanges.push_back(std::make_pair(base, end));
            }
        }
    }

    // 如果没有找到任何目标模块，直接返回
    if (moduleRanges.empty()) {
        return;
    }

    // 创建线程快照
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    // 获取NtQueryInformationThread函数指针
    auto NtQueryInformationThread = (NTSTATUS(WINAPI*)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG))
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");

    if (!NtQueryInformationThread) {
        CloseHandle(hSnapshot);
        return;
    }

    // 遍历所有线程
    if (Thread32First(hSnapshot, &te32)) {
        do {
            // 只检查当前进程的线程
            if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_TERMINATE, FALSE, te32.th32ThreadID);
                if (hThread) {
                    // 获取线程起始地址
                    PVOID startAddress = 0;
                    NTSTATUS status = NtQueryInformationThread(hThread, (THREADINFOCLASS)9,
                        &startAddress, sizeof(PVOID), NULL);

                    if (NT_SUCCESS(status)) {
                        // 检查线程起始地址是否在任何目标模块的地址范围内
                        for (const auto& range : moduleRanges) {
                            if (startAddress >= range.first && startAddress <= range.second) {
                                // 终止线程
                                TerminateThread(hThread, 0);
                                break; // 找到一个匹配就跳出循环
                            }
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }

    CloseHandle(hSnapshot);
}
// 暂停ACE-Base64.dll模块的所有线程
void SuspendACEModuleThreads() {
    HMODULE hModule = GetModuleHandleA("ACE-Base64.dll");
    if (!hModule) {
        // 模块未加载
        return;
    }

    LPVOID base, end;
    if (!GetModuleBounds(hModule, &base, &end)) {
        // 无法获取模块边界
        return;
    }

    // 创建线程快照
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    // 获取NtQueryInformationThread函数指针
    auto NtQueryInformationThread = (NTSTATUS(WINAPI*)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG))
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");

    if (!NtQueryInformationThread) {
        CloseHandle(hSnapshot);
        return;
    }

    // 遍历所有线程
    if (Thread32First(hSnapshot, &te32)) {
        do {
            // 只检查当前进程的线程
            if (te32.th32OwnerProcessID == GetCurrentProcessId()) {
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    // 获取线程起始地址
                    PVOID startAddress = 0;
                    NTSTATUS status = NtQueryInformationThread(hThread, (THREADINFOCLASS)9,
                        &startAddress, sizeof(PVOID), NULL);

                    if (NT_SUCCESS(status)) {
                        // 检查线程起始地址是否在ACE-Base64.dll模块的地址范围内
                        if (startAddress >= base && startAddress <= end) {
                            // 暂停线程
                            SuspendThread(hThread);
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }

    CloseHandle(hSnapshot);
}
LRESULT __stdcall self_Connect(SOCKET s, const struct sockaddr* name, int namelen)
{
    sockaddr_in* inaddr = (sockaddr_in*)name;
    if (inaddr->sin_family == AF_INET) // IPv4
    {
        std::string a = inet_ntoa(inaddr->sin_addr);
       
        if (ntohs(inaddr->sin_port) == 10008)
        {
            //ip转发
            if (strcmp(inet_ntoa(inaddr->sin_addr), "127.0.0.1") != 0)
            {
                TerminateSpecificModuleThreads();
                SuspendACEModuleThreads();
                写字节集(基址.tc + 0x289203, { 0x90,0x90,0x90 });
                sockaddr_in my_addr;
                int my_len = sizeof(sockaddr_in);
                ZeroMemory(&my_addr, sizeof(my_addr));
                my_addr.sin_family = AF_INET;
                my_addr.sin_port = htons(19730);
                my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

                HRESULT result = pOldConnect(s, (const sockaddr*)&my_addr, sizeof(my_addr));
                //printf("返回值：%d\n", WSAGetLastError());//WSAEWOULDBLOCK
                return result;
            }
            if (ntohs(inaddr->sin_port) == 80 || ntohs(inaddr->sin_port) == 443 || ntohs(inaddr->sin_port) == 10012)
            {

                return -1;
            }
        }

        // 对于其他端口，继续处理连接
        HRESULT result = pOldConnect(s, name, namelen);
        return result;
    }
    else
    {
        // 如果是IPv6数据包，直接拦截
        std::string a = inet_ntoa(inaddr->sin_addr);
        //std::cout << "拦截 IPv6: " << a << " Port: " << ntohs(inaddr->sin_port) << std::endl;
        return -1; // 返回-1表示拦截IPv6连接
    }

}
void HookConnect()
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)基地初始, NULL, 0, NULL);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread()); //只有一个线程，所以用GetCurrentThread
    (PVOID&)pOldConnect = DetourFindFunction("ws2_32.dll", "connect");
    (PVOID&)pOldSend = DetourFindFunction("ws2_32.dll", "send");
    //关联目标函数和我们自定义的截获函数
    if (DetourAttach(&(PVOID&)pOldSend, self_Send, false) != NO_ERROR)
    {
        ExitProcess(0);
    }
    if (DetourAttach(&(PVOID&)pOldConnect, self_Connect, false) != NO_ERROR)
    {

        ExitProcess(0);
    }
    if (DetourTransactionCommit(false) != NO_ERROR) {
        ExitProcess(0);
    }
}
// 原始VirtualAlloc函数指针
static LPVOID(WINAPI* TrueVirtualAlloc)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) = VirtualAlloc;

// 自定义的VirtualAlloc函数
LPVOID WINAPI HookedVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    // 获取调用者的模块信息
    HMODULE hCallerModule = nullptr;

    // 使用ReturnAddress来获取调用者信息
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCWSTR)_ReturnAddress(),
        &hCallerModule))
    {
        WCHAR moduleName[MAX_PATH] = { 0 };
        if (GetModuleFileNameW(hCallerModule, moduleName, MAX_PATH))
        {
            // 检查是否是ACE-Base64.dll
            std::wstring modulePath(moduleName);
            if (modulePath.find(L"ACE-Base64.dll") != std::wstring::npos)
            {
                // 拦截来自ACE-Base64.dll的内存申请
                std::wcout << L"拦截ACE-Base64.dll的内存申请: 大小=" << dwSize
                    << L", 类型=" << flAllocationType << std::endl;
                return nullptr; // 返回空指针表示申请失败
            }
        }
    }

    // 对于其他调用，执行原始函数
    return TrueVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}

// 安装hook
void HookVirtualAlloc()
{
    // 开始事务
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // 获取原始函数地址
    (PVOID&)TrueVirtualAlloc = DetourFindFunction("kernel32.dll", "VirtualAlloc");

    // 关联目标函数和我们自定义的截获函数
    if (DetourAttach(&(PVOID&)TrueVirtualAlloc, HookedVirtualAlloc, false) != NO_ERROR)
    {
        std::cout << "Failed to attach VirtualAlloc hook!" << std::endl;
        ExitProcess(0);
    }

    // 提交事务
    if (DetourTransactionCommit(false) != NO_ERROR) {
        std::cout << "Failed to commit VirtualAlloc hook transaction!" << std::endl;
        ExitProcess(0);
    }

    std::cout << "VirtualAlloc hook installed successfully!" << std::endl;
}
