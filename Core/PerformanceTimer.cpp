#include "PerformanceTimer.h"

PerformanceTimer::PerformanceTimer()
{
	QueryPerformanceFrequency(&frequency_);
	Reset();
}

void PerformanceTimer::Reset()
{
	QueryPerformanceCounter(&lastCount_);
}

float PerformanceTimer::Tick()
{
	auto prevCountQuadPart = lastCount_.QuadPart;
	QueryPerformanceCounter(&lastCount_);
	auto diff = lastCount_.QuadPart - prevCountQuadPart;
	return static_cast<float>(diff * 1000000 / frequency_.QuadPart) / 1000000.0f;
}
