#include "FantacyEngine.h"
#include "Engine.h"
static Engine* app;

__declspec(dllexport) int Initialize(int a)
{
	if (a > 10) {
		return 1;
	}
	return 0;
}

__declspec(dllexport) void Shutdown()
{
	app->OnDestroy();
	delete app;
}

__declspec(dllexport) void SetRenderWindow(HWND hWnd, int x, int y, int width, int height)
{
	app = new Engine;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	app->Run(hInstance);
}

__declspec(dllexport) void Render()
{
}
