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
        m_start = timeGetTime();        // �v���J�n���Ԃ�ۑ�
    }
    double  elapsed()    // ���X�^�[�g����̕b����Ԃ�
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
    /// �^�C�}�[��MaxTime�ɓ��B�������ǂ���
    /// </summary>
    /// <param name="Maxtime">�I���t���[��</param>
    /// <returns></returns>
    bool IsTimeOut(const float Maxtime);

    /// <summary>
    /// �Z�b�g�ő厞��
    /// </summary>
    /// <param name="maxtime">�ő厞��</param>
    /// <returns></returns>
    float& SetMaxTime(float maxtime) { return this->maxtime = maxtime; }

    float& GetMaxtime() { return maxtime; }
private:
    DWORD  m_start;    //  �v���J�n����
    float elapsedTime;
    float timeRate;
    float totalTime;
    float maxtime;
};