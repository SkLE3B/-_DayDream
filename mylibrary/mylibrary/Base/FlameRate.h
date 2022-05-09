#pragma once
class FlameRate
{
public: FlameRate();
	  void Update();
	  void Wait();
private:
	int startTime;
	int count;
	int FPS;
	int TookTime;
	int WaitTime;
};

