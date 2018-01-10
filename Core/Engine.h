#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include <string>
#include <wrl.h>

#include "PerformanceTimer.h"
#include "Graphics3D.h"

using Microsoft::WRL::ComPtr;
class Engine
{
public:
	void Run(HINSTANCE hInstance);

	void SetResolution(int xResolution, int yResolution);
	void SetWindowSize(int width, int height);
	void SwitchToFullScreen();

	void OnCreate();
	void OnUpdate();
	void OnRender();
	void OnDestroy();
private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ResetTimer();
private:
	// engine logic
	PerformanceTimer timer_;
	// engine systems
	Graphics3D graphics3D_;
	// low level variables
	HWND hWnd_;
};