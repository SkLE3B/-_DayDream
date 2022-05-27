#pragma once
#include "../mylibrary/Base/Timer.h"

class HelperTimer
{
public:
	//<summary>
    //タイマーを起動する
    //</summary>
	static void TimerStart();

	/// <summary>
	/// タイマーを進める
	/// </summary>
	/// <param name="maxTime">最大時間</param>
	static void AdvanceTimer(const float maxTime);

	/// <summary>
	/// タイマーを止める
	/// </summary>
	static void ResetTimer();

	/// <summary>
	/// タイマーがMaxTimeに到達したかどうか
	/// </summary>
	/// <param name="TotalTime">カウントしているタイマー</param>
	/// <param name="Maxtime">終了フレーム</param>
	/// <returns></returns>
	bool IsTimeOut(float TotalTime, const float Maxtime) const;

	/// <summary>
    /// イージングが終わったかどうか
    /// </summary>
    /// <returns></returns>
	static bool IsEasingOver();

	/// <summary>
	/// タイマー起動中かどうか
	/// </summary>
	/// <returns>timerOn</returns>
	bool IsTimerOn() const { return timerOn == true; }

	/// <summary>
	/// Getter
	/// </summary>
	/// <returns>totalTime</returns>
	static float& GetTotalTime() { return totalTime; }

private:
	static Timer* time;
	static float timeRate;  //何％時間が進んだか(率)
	static float elapsedTime;//何％時間が進んだか(値)
	static float totalTime;  //進めた時間
	static bool timerOn;
};