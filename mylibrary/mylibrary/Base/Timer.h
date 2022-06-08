#pragma once

#include <windows.h>
#pragma comment(lib, "winmm.lib")

class Timer
{
public:
    Timer() { restart(); }
public:
    void  restart()
    {
        m_start = timeGetTime();        // 計測開始時間を保存
    }
    double  elapsed()    // リスタートからの秒数を返す
    {
        DWORD end = timeGetTime();
        return (float)(end - m_start) / 10000.0f;
    }

    void Update();

    void Reset();

    bool IsTimer();

    bool IsEasingOver();

    float GetCurrentTimer() { return totalTime; }

    float& GetMaxTime(float value) { return maxtime = value; }

    float& GetMaxTime() { return maxtime; }

    float& GetTimeRate() { return timeRate; }

    /// <summary>
    /// タイマーがMaxTimeに到達したかどうか
    /// </summary>
    /// <param name="Maxtime">終了フレーム</param>
    /// <returns></returns>
    bool IsTimeOut(const float Maxtime);

    /// <summary>
    /// セット最大時間
    /// </summary>
    /// <param name="maxtime">最大時間</param>
    /// <returns></returns>
    float& SetMaxTime(float maxtime) { return this->maxtime = maxtime; }

    float& GetMaxtime() { return maxtime; }
private:
    DWORD  m_start;    //  計測開始時間
    float elapsedTime;
    float timeRate;
    float totalTime;
    float maxtime;
};