#include "Timer.h"
#include <math.h>

Timer::Timer(void)
{
	QueryPerformanceFrequency(&mFrequency);
	QueryPerformanceCounter(&mPrevTime);
	QueryPerformanceCounter(&mPrevSecondTime);
	mFrameCounter = 0;
}

float Timer::GetDeltaTime(void)
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	float deltaTime = static_cast<float>(currentTime.QuadPart - mPrevTime.QuadPart) / mFrequency.QuadPart;
	mPrevTime = currentTime;

	return deltaTime;
}

void Timer::PrintFPS(void)
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	++mFrameCounter;
	if (currentTime.QuadPart - mPrevSecondTime.QuadPart > mFrequency.QuadPart)
	{
		QueryPerformanceCounter(&mPrevSecondTime);
		char buf[0x20] = { 0, };
		buf[0] = 'F';
		buf[1] = 'P';
		buf[2] = 'S';
		buf[3] = ':';
		buf[4] = 0x20;
		int digits = static_cast<int>(ceilf(log10f(static_cast<float>(mFrameCounter))));
		_itoa_s(mFrameCounter, buf + 5, sizeof(buf) - 5, 10);
		buf[digits + 5] = 0xa;
		buf[digits + 6] = 0;
		OutputDebugStringA(buf);
		mFrameCounter = 0;
	}
}
