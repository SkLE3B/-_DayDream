#include "FlameRate.h"
#include "time.h"
#include <windows.h>
FlameRate::FlameRate()
{
	FPS = 60;//固定したいフレームレート
	count = 0;//フレーム数
	startTime = 0;//処理を開始してからの時間の記録
}
void FlameRate::Update()
{
	if (count == 0)
	{
		startTime = clock();
	}
	if (count == 60)
	{
		startTime = clock();
		count = 0;
	}
	count++;
}
void FlameRate::Wait()
{
	TookTime = clock() - startTime;
	WaitTime = count * 1000 / FPS - TookTime;
	if (WaitTime > 0)
	{
		Sleep(WaitTime);
	}
}