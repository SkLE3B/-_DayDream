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
private:
    DWORD    m_start;    //  �v���J�n����
};