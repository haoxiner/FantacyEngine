#pragma once
#include <Windows.h>
class PerformanceTimer {
public:
	PerformanceTimer();
	~PerformanceTimer() = default;
	void Reset();
	float Tick();
private:
	LARGE_INTEGER lastCount_;
	LARGE_INTEGER frequency_;
};