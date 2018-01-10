#include "Engine.h"

void Engine::Run(HINSTANCE hInstance)
{
	const int DEFAULT_WIDTH = 1024, DEFAULT_HEIGHT = 768;

	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"EngineClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(DEFAULT_WIDTH), static_cast<LONG>(DEFAULT_HEIGHT) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	hWnd_ = CreateWindow(
		windowClass.lpszClassName,
		L"Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,		// We have no parent window.
		nullptr,		// We aren't using menus.
		hInstance,
		this); // Pass engine instance to WindowProc

	graphics3D_.Initialize(hWnd_, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	ShowWindow(hWnd_, SW_SHOW);

	// Main loop.
	MSG msg = {};
	ResetTimer();
	while (msg.message != WM_QUIT) {
		// Process any messages in the queue.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		OnUpdate();
	}
}

void Engine::SetResolution(int xResolution, int yResolution)
{
}

void Engine::SetWindowSize(int width, int height)
{
}

void Engine::SwitchToFullScreen()
{
}

void Engine::OnUpdate()
{
	float deltaTime_ = timer_.Tick();
}

void Engine::OnRender()
{
	graphics3D_.Render();
}

void Engine::OnDestroy()
{
	graphics3D_.Destroy();
}

LRESULT CALLBACK Engine::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Engine* engine = reinterpret_cast<Engine*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message) {
	case WM_CREATE:
	{
		// Save the DXSample* passed in to CreateWindow.
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	return 0;

	case WM_KEYDOWN:
		if (engine) {
			//graphics3D->OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_KEYUP:
		if (engine) {
			//graphics3D->OnKeyUp(static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_PAINT:
		if (engine) {
			engine->OnRender();
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Engine::ResetTimer()
{
	timer_.Reset();
}
