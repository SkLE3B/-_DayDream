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
private:
    DWORD    m_start;    //  計測開始時間
};