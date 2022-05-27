#include "HelperTimer.h"

Timer* HelperTimer::time = nullptr;
float HelperTimer::timeRate;
float HelperTimer::elapsedTime;
float HelperTimer::totalTime;
bool HelperTimer::timerOn;

void HelperTimer::TimerStart()
{
	timerOn = !timerOn;

	if (time == nullptr)
	{
		time = new Timer();
	}
	else
	{
		time->restart();
	}
}

void HelperTimer::AdvanceTimer(const float maxTime)
{
	elapsedTime = time->elapsed();
	totalTime += elapsedTime;
	timeRate = min(totalTime / maxTime, 1.0f);
}

void HelperTimer::ResetTimer()
{
	timeRate    = 0;
	elapsedTime = 0;
	totalTime   = 0;
}

bool HelperTimer::IsTimeOut(float TotalTime, const float Maxtime) const
{
	return TotalTime >= Maxtime;
}

bool HelperTimer::IsEasingOver()
{
	const float EasingOver = 1.0f;//timeRate‚ª1‚É‚È‚é‚ÆEasingI—¹

	return timeRate == EasingOver;
}