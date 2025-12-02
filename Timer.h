#pragma once
#include <Windows.h>

class Timer
{
public:
	Timer(void);
	~Timer(void) {};
	float GetDeltaTime(void);
	void PrintFPS(void);
private:
	// timer
	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mPrevTime;
	LARGE_INTEGER mPrevSecondTime;
	size_t mFrameCounter;
};

