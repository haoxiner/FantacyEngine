#include "FantacyEngine.h"
#include "EngineSample.h"
static EngineSample* app;

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
	app = new EngineSample(hWnd, x, y, width, height);
	app->OnCreate();
}

__declspec(dllexport) void Render()
{
	app->OnRender();
}
