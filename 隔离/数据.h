#pragma once
#include "232.h"

static HANDLE   globalFylHandle = 0;
int 自瞄范围;
int 瞄准速率;
INT32 端口1 = 3306;
float TargetX = 0, TargetY = 0, Frevise;
typedef struct _IntersectInfo
{
	float vImpactPosX;
	float vImpactPosY;
	float vImpactPosZ;
	char spacer00[32];
	__int64 hObjImpact;
	unsigned long unk1;
	unsigned long unk2;
	unsigned long unk3;
}IntersectInfo;
typedef struct _IntersectQuery
{
	float m_FromX;
	float m_FromY;
	float m_FromZ;
	float m_ToX;
	float m_ToY;
	float m_ToZ;
	__int64 m_Flags;
	void* m_FilterFn;
	__int64 m_FilterActualIntersectFn;
	__int64 m_PolyFilterFn;
	void* m_pUserData;
	void* m_pActualIntersectUserData;
}IntersectQuery;
IntersectInfo Info;
IntersectQuery Query;
bool 取生化僵尸图标(int a)
{
	ULONG64 敌人数据 = 读长整数(基址.敌人地址 + 24 + (a - 1) * 基址.数组大小);

	if (读整数型(敌人数据 + 0x11460) > 0)
	{
		return true;
	}


	return false;

}
bool 障碍Call(D3D坐标 MePos, D3D坐标 TargetPos)
{
	memset(&Query, 0, sizeof(_IntersectQuery));
	memset(&Info, 0, sizeof(_IntersectInfo));
	Query.m_FromX = MePos.x;
	Query.m_FromY = MePos.z;
	Query.m_FromZ = MePos.y;
	Query.m_ToX = TargetPos.x;
	Query.m_ToY = TargetPos.z;
	Query.m_ToZ = TargetPos.y;
	return !((bool(__fastcall*)(IntersectQuery&, IntersectInfo*))基址.障碍基址)(Query, &Info);
}
int 取人物数量()
{
	__int64 模式 = 读整数型(基址.模式地址);

	if (模式 == 41 || 模式 == 47 || 模式 == 4 || 模式 == 36 || 模式 == 49)//个人
	{
		return 16;//个人
	}

	if (模式 == 14 || 模式 == 12 || 模式 == 5 || 模式 == 17 || 模式 == 19 || 模式 == 47)//生化丨小丑丨暗影（感染）
	{
		return 30;//个人
	}
	return 8;
}
int 取自己位置()
{
	return 读长整数((读长整数(基址.玩家数组) + 基址.本人ID)) + 1;
}
void 取本人坐标(D3D坐标* 返回数据)
{

	返回数据->x = 读小数型((0x14060B510));
	返回数据->z = 读小数型((0x14060B510 + 4));
	返回数据->y = 读小数型((0x14060B510 + 8));


}
BOOL WorldToScreen(D3D坐标 敌人坐标, D3D坐标* 返回坐标)
{
	BYTE view[64] = { NULL }, projection[64] = { NULL }, viewPort[64] = { NULL };
	BYTE World[64] = { 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63 };
	D3DXVECTOR3	vScreen, Pos2, PlayerPos;
	__int64 ss = 基址.矩阵地址;
	PlayerPos.x = 敌人坐标.x;
	PlayerPos.y = 敌人坐标.z;
	PlayerPos.z = 敌人坐标.y;
	读字节集((ss - 128), 64, view);
	读字节集((ss - 64), 64, projection);
	读字节集(ss, 64, viewPort);
	D3DXVec3Project(&vScreen, &PlayerPos, (D3DVIEWPORT9*)&viewPort, (D3DXMATRIX*)&projection, (D3DXMATRIX*)&view, (D3DXMATRIX*)&World);
	if (vScreen.z > 1)
	{
		return false;
	}
	返回坐标->x = vScreen.x;
	返回坐标->y = vScreen.y;
	return true;
}
bool 是否在游戏()
{
	int a = 0;
	a = 读整数型(基址.是否游戏);
	if (a != (int)1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
//bool 取敌人生存(__int64 a)
//{
//	__int64 敌人基址x = NULL, Plaply_INFOx;
//
//	// 如果敌人基址x是NULL，重新赋值
//	if (敌人基址x == 0)
//	{
//		敌人基址x = 基址.敌人地址 + 24;
//	}
//
//	// 计算Plaply_INFOx的值
//	Plaply_INFOx = 读长整数(敌人基址x + (a - 1) * 基址.数组大小);
//
//	// 如果Plaply_INFOx为0，表示无效地址，返回FALSE
//	if (Plaply_INFOx == 0)
//	{
//		return FALSE;
//	}
//
//	// 确保读取的地址有效，并且防止越界
//	if (Plaply_INFOx + 基址.生存偏移 >= 0x900000000 || Plaply_INFOx == NULL)
//	{
//		return FALSE;  // 如果地址无效，返回FALSE
//	}
//
//	// 读取生存状态，如果偏移位置的字节值为0，表示敌人已死亡
//	if (读字节型(Plaply_INFOx + 基址.生存偏移) == (BYTE)0)
//	{
//		return TRUE;  // 敌人已死亡
//	}
//
//	return FALSE;  // 敌人还活着
//}

__int64 取自己生存()
{
	__int64 ret = 读长整数((基址.人物地址 - 8));
	return ret;
}
float 计算2D距离(int 本人x, int 本人y, float 敌人x, float 敌人y)
{


	本人x = 本人x - 敌人x;
	本人y = 本人y - 敌人y;


	double 距离 = sqrt(本人x * 本人x + 本人y * 本人y);

	return 距离;
}
bool 取骨骼坐标(__int64 ID, D3D坐标* 寄存, __int64 位置)
{
	ULONG64 addsr = Read<ULONG64>(基址.玩家数组);
	ULONG64 addr1 = Read<ULONG64>(addsr + (ID - 1) * 8 + 0x10);

	ULONG64 addr2 = Read<ULONG64>(addr1 + 8);

	ULONG64 addr3 = Read<ULONG64>(addr1 + 0x20);

	if (addr2 < 65536) {
		return FALSE;
	}
	if (addr3 < 65536) {
		return FALSE;
	}
	ULONG64 ZZ = (Read<ULONG64>(addr2 + 0x29B8) + 12) + (64 * 位置);
	寄存->x = 读小数型(ZZ);
	寄存->z = 读小数型((ZZ + 16));
	寄存->y = 读小数型((ZZ + 32));
	return TRUE;
}

int 取敌人距离1(int 鼠标X, int 鼠标Y, int 敌人2DX, int 敌人2DY)
{
	int 距离 = 0;
	鼠标X = 鼠标X - 敌人2DX;
	鼠标Y = 鼠标Y - 敌人2DY;


	距离 = sqrt(鼠标X * 鼠标X + 鼠标Y * 鼠标Y);
	return 距离;
}
BOOL 世界屏幕(D3D坐标 敌人坐标, D3D坐标* 返回坐标)
{
	byte view[64] = { NULL }, projection[64] = { NULL }, viewPort[64] = { NULL };
	byte World[64] = { 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63 };
	D3DXVECTOR3	vScreen, Pos2, PlayerPos;

	PlayerPos.x = 敌人坐标.x;
	PlayerPos.y = 敌人坐标.z;   //z
	PlayerPos.z = 敌人坐标.y;   //y

	memcpy(&viewPort, reinterpret_cast<void*>(基址.矩阵地址), 64);
	memcpy(&view, reinterpret_cast<void*>(基址.矩阵地址 - 128), 64);
	memcpy(&projection, reinterpret_cast<void*>(基址.矩阵地址 - 64), 64);

	//旋转项目 D3DXVec3Project 输出坐标  输入坐标  视口  投影矩阵  视变换矩阵  世界变换矩
	D3DXVec3Project(&vScreen, &PlayerPos, (D3DVIEWPORT9*)&viewPort, (D3DXMATRIX*)&projection, (D3DXMATRIX*)&view, (D3DXMATRIX*)&World);
	if (vScreen.z > 1)

	{
		return false;
	}
	返回坐标->x = vScreen.x;
	返回坐标->y = vScreen.y;
	return true;
}
float 取_对象2D距离(float A鼠标X, float A鼠标Y, float 敌人2DX, float 敌人2DY)
{
	float 距离A;
	A鼠标X = A鼠标X - 敌人2DX;
	A鼠标Y = A鼠标Y - 敌人2DY;
	距离A = sqrt(A鼠标X * A鼠标X + A鼠标Y * A鼠标Y);
	return 距离A;
}


void 判断准心距离(D3D坐标 CCC, int id)
{
	int 方框中心X = CCC.x;
	int 方框中心Y = CCC.y;

	数据.无敌 = 取敌人距离1(数据.游戏宽度 / 2, 数据.游戏高度 / 2, 方框中心X, 方框中心Y);

	if (数据.锁定pawn == 0) {
		数据.锁定pawn = id;
		最近距离 = 数据.无敌;
	}
	else if (数据.无敌 < 最近距离) {
		数据.锁定pawn = id;
		最近距离 = 数据.无敌;
	}

}
屏幕坐标 转屏幕方框(int n)
{
	屏幕坐标 绘制 = {};
	D3D坐标 坐标, 脚底, 头顶;
	if (取骨骼坐标(n, &坐标, 0))
	{
		坐标.z = 坐标.z - 140;
		if (世界屏幕(坐标, &脚底))
		{
			if (取骨骼坐标(n, &坐标, 6))
			{
				坐标.z = 坐标.z + 37;
				if (世界屏幕(坐标, &头顶))
				{
					绘制.h = (int)(脚底.y - 头顶.y);
					绘制.w = (int)(绘制.h / 2);
					绘制.x = (int)(头顶.x - 绘制.h / 4);
					绘制.y = (int)(头顶.y);
				}
			}
		}
	}
	return 绘制;
}
int 取人物血量(int a)
{

	ULONG64 addsr = Read<ULONG64>(基址.玩家数组);
	ULONG64 addr1 = Read<ULONG64>(addsr + (a - 1) * 8 + 0x10);
	int xl = Read<int>(addr1 + 0x4c);
	return (xl);
}
bool 是否炸弹(__int64 a)
{

	ULONG64 addsr = Read<ULONG64>(基址.玩家数组);
	ULONG64 addr1 = Read<ULONG64>(addsr + (a - 1) * 8 + 0x10);
	byte 是否带包 = 读字节型(addr1 + 0x30);
	if (是否带包 == 0x1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
const char* 取敌人昵称(__int64 ID)
{
	ULONG64 addsr = Read<ULONG64>(基址.玩家数组);
	ULONG64 addr1 = Read<ULONG64>(addsr + (ID - 1) * 8 + 0x10);
	return  读文本型(addr1+0x12);
}
void 锁定范围(D3D坐标 坐标, int 目标)
{
	int 画出宽度, 画出高度, 距离;
	float 瞄准角度X, 瞄准角度Y, 瞄准角度H, 瞄准角度W;
	画出宽度 = 坐标.x;
	画出高度 = 坐标.y;
	瞄准角度X = 数据.游戏宽度 / 2 - 自瞄范围 + 自瞄范围 / 5.5;
	瞄准角度Y = 数据.游戏高度 / 2 - 自瞄范围 + 自瞄范围 / 5.5;
	瞄准角度H = 自瞄范围 * 2 - 自瞄范围 / 3;
	瞄准角度W = 自瞄范围 * 2 - 自瞄范围 / 3;
	距离 = sqrt((画出宽度 - 数据.游戏宽度 / 2) * (画出宽度 - 数据.游戏宽度 / 2) + (画出高度 - 数据.游戏高度 / 2) * (画出高度 - 数据.游戏高度 / 2));
	if (画出宽度 > 瞄准角度X && 画出宽度 < 瞄准角度X + 瞄准角度W)
	{
		if (画出高度 > 瞄准角度Y && 画出高度 < 瞄准角度Y + 瞄准角度H)
		{
			if (数据.锁定pawn == 0)
			{
				数据.锁定pawn = 目标;
				数据.上次距离 = 距离;
			}
			else if (距离 < 数据.上次距离)
			{
				数据.锁定pawn = 目标;
			}
		}
	}
}
DWORD 取敌人距离(D3D坐标 My, D3D坐标 Target) {
	D3D坐标 差距;
	差距.x = My.x * -1 - Target.x * -1;
	差距.y = My.y - Target.y;
	差距.z = My.z - Target.z;
	return sqrt(差距.x * 差距.x + 差距.y * 差距.y + 差距.z * 差距.z) / 100;
}

D3D坐标 VectorToRotation(D3D坐标 自己坐标, D3D坐标 b)
{
	double X差距, Y差距, Z差距, X角度, Y角度, 邻边, 对边;
	D3D坐标 锁定坐标, 敌人坐标;
	敌人坐标 = b;
	const double pi = 3.1415926535;
	X差距 = 自己坐标.x * -1 - 敌人坐标.x * -1;
	Y差距 = 自己坐标.y - 敌人坐标.y;
	Z差距 = 自己坐标.z - 敌人坐标.z;
	if (X差距 == 0 && Y差距 < 0)
	{
		X角度 = 0;
	}
	else if (X差距 == 0 && Y差距 > 0)
	{
		X角度 = 180;
	}
	else if (X差距 > 0 && Y差距 == 90)
	{
		X角度 = 90;
	}
	else if (X差距 < 0 && Y差距 == 0)
	{
		X角度 = 270;
	}
	else if (X差距 > 0 && Y差距 < 0)
	{
		对边 = Y差距;
		邻边 = X差距;
		X角度 = atan(对边 / 邻边) * 180 / pi + 90;
	}
	else if (X差距 < 0 && Y差距 < 0)
	{
		对边 = X差距;
		邻边 = Y差距;
		X角度 = 360 - atan(对边 / 邻边) * 180 / pi;
	}
	else if (X差距 > 0 && Y差距 > 0)
	{
		对边 = Y差距;
		邻边 = X差距;
		X角度 = atan(对边 / 邻边) * 180 / pi + 90;
	}
	else if (X差距 < 0 && Y差距>0)
	{
		对边 = X差距;
		邻边 = Y差距;
		X角度 = 180 - atan(对边 / 邻边) * 180 / pi;
	}
	else
	{
		X角度 = 0;
	}
	Y角度 = atan(Z差距 / sqrt(Y差距 * Y差距 + X差距 * X差距)) * 180 / pi;
	锁定坐标.x = X角度 * pi / 180;
	锁定坐标.y = Y角度 * pi / 180;
	锁定坐标.z = 0;
	//printf("vector返回的锁定坐标x:%f, y:%f\n", 锁定坐标.x, 锁定坐标.y);
	return 锁定坐标;
}
D3D坐标 自瞄矩阵(D3D坐标 敌人坐标)
{
	BYTE World[64] = { 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63 };
	BYTE view[64], projection[64], viewPort[64];
	D3DXVECTOR3 vScreen, Pos2, PlayerPos, abcd, 差距;
	double 距离 = 0, 倍数x = 0;
	PlayerPos.x = 敌人坐标.x;
	PlayerPos.y = 敌人坐标.z + 18.8;
	PlayerPos.z = 敌人坐标.y;

	memcpy(&viewPort, (const void*)(基址.矩阵地址), 64);
	memcpy(&view, (const void*)(基址.矩阵地址 - 128), 64);
	memcpy(&projection, (const void*)(基址.矩阵地址 - 64), 64);

	D3DXVec3Project(&vScreen, &PlayerPos, (D3DVIEWPORT9*)&viewPort, (D3DXMATRIX*)&projection, (D3DXMATRIX*)&view, (D3DXMATRIX*)&World);
	敌人坐标.x = vScreen.x;
	敌人坐标.y = vScreen.y;
	敌人坐标.z = vScreen.z;
	return 敌人坐标;
}
bool 鼠标移动(int LastX, int LastY, int Flags)
{
	INPUT Input;
	Input.type = INPUT_MOUSE;
	Input.mi.mouseData = 0;
	Input.mi.dwExtraInfo = 0;
	if (Flags == 0)
	{
		Input.mi.dx = LastX;
		Input.mi.dy = LastY;
		Input.mi.dwFlags = 1 | Flags;
	}
	if (Flags == 1)
	{
		Input.mi.dx = LastX * 65535 / GetSystemMetrics(0);
		Input.mi.dy = LastY * 65535 / GetSystemMetrics(1);
		Input.mi.dwFlags = 32768 | Flags;
	}
	return SendInput(1, &Input, sizeof(Input)) > 0;
}

void 自瞄算法(D3D坐标 Output)
{
	Frevise = (float)0.3;
	if (Output.x != 0)
	{
		if (Output.x > 数据.游戏中心X)
		{
			TargetX = -(数据.游戏中心X - Output.x);
			TargetX = TargetX / 瞄准速率;
			if (TargetX + 数据.游戏中心X + 1 > 数据.游戏宽度)
			{
				TargetX = 0;
			}
			if (TargetX<1 && TargetX>Frevise)
			{
				TargetX = 1;
			}
		}
		if (Output.x < 数据.游戏中心X)
		{
			TargetX = Output.x - 数据.游戏中心X;
			TargetX = TargetX / 瞄准速率;
			if (TargetX + 数据.游戏中心X - 1 < 0)
			{
				TargetX = 0;
			}
			if (TargetX > -1 && TargetX < Frevise)
			{
				TargetX = -1;
			}
		}
	}
	if (Output.y != 0)
	{
		if (Output.y > 数据.游戏中心Y)
		{
			TargetY = -(数据.游戏中心Y - Output.y);
			TargetY = TargetY / 瞄准速率;
			if (TargetY + 数据.游戏中心Y + 1 > 数据.游戏高度)
			{
				TargetY = 0;
			}
			if (TargetY<1 && TargetY>Frevise)
			{
				TargetY = 1;
			}
		}
		if (Output.y < 数据.游戏中心Y)
		{
			TargetY = Output.y - 数据.游戏中心Y;
			TargetY = TargetY / 瞄准速率;
			if (TargetY + 数据.游戏中心Y - 1 < 0)
			{
				TargetY = 0;
			}
			if (TargetY > -1 && TargetY < Frevise)
			{
				TargetY = -1;
			}
		}
	}
	mouse_event(1, TargetX, TargetY, 3.2, 3.2);
}
int 随机位置()
{
	for (int i = 0; i < 6; i++)
	{
		return rand() % 10;
	}
	return 0;
}
BYTE* MovRax(__int64 Address)
{
	BYTE* RetBytes = NULL;
	BYTE TempBytes[10] = { 0x48,0xA1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	memcpy(&TempBytes[0] + 0x02, &Address, 8);
	return TempBytes;
}
BYTE* 到字节集(DWORD64 num)
{
	BYTE bytes[8] = {};
	bytes[0] = num;
	bytes[1] = num >> 8;
	bytes[2] = num >> 16;
	bytes[3] = num >> 24;
	bytes[4] = num >> 32;
	bytes[5] = num >> 40;
	bytes[6] = num >> 48;
	bytes[7] = num >> 56;
	return bytes;
}
