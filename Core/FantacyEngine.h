#pragma once
#include <Windows.h>
extern "C"
{
	__declspec(dllexport) int Initialize(int a);
	__declspec(dllexport) void Shutdown();
	__declspec(dllexport) void SetRenderWindow(HWND hWnd, int x, int y, int width, int height);
	__declspec(dllexport) void Render();
}