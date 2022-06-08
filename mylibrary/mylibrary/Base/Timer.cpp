#include "Timer.h"

void Timer::Update()
{
	elapsedTime = elapsed();
	totalTime += elapsedTime;
	timeRate = min(totalTime / GetMaxtime(), 1.0f);
}

void Timer::Reset()
{
	const float resetValue = 0;
	totalTime = 0;
	elapsedTime = 0;
	timeRate = 0;
}

bool Timer::IsTimer()
{
	bool timeCount;

	if (GetCurrentTimer() > 0)
	{
		timeCount = true;
	}
	else
	{
		timeCount = false;
	}

	return timeCount;
}

bool Timer::IsEasingOver()
{
	const float EasingOver = 1.0f;//timeRate‚ª1‚É‚È‚é‚ÆEasingI—¹
	return GetTimeRate() == EasingOver;
}

bool Timer::IsTimeOut(const float Maxtime) 
{
	return GetCurrentTimer() >= Maxtime;
}
