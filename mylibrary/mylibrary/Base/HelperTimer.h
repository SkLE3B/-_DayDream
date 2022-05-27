#pragma once
#include "../mylibrary/Base/Timer.h"

class HelperTimer
{
public:
	//<summary>
    //�^�C�}�[���N������
    //</summary>
	static void TimerStart();

	/// <summary>
	/// �^�C�}�[��i�߂�
	/// </summary>
	/// <param name="maxTime">�ő厞��</param>
	static void AdvanceTimer(const float maxTime);

	/// <summary>
	/// �^�C�}�[���~�߂�
	/// </summary>
	static void ResetTimer();

	/// <summary>
	/// �^�C�}�[��MaxTime�ɓ��B�������ǂ���
	/// </summary>
	/// <param name="TotalTime">�J�E���g���Ă���^�C�}�[</param>
	/// <param name="Maxtime">�I���t���[��</param>
	/// <returns></returns>
	bool IsTimeOut(float TotalTime, const float Maxtime) const;

	/// <summary>
    /// �C�[�W���O���I��������ǂ���
    /// </summary>
    /// <returns></returns>
	static bool IsEasingOver();

	/// <summary>
	/// �^�C�}�[�N�������ǂ���
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
	static float timeRate;  //�������Ԃ��i�񂾂�(��)
	static float elapsedTime;//�������Ԃ��i�񂾂�(�l)
	static float totalTime;  //�i�߂�����
	static bool timerOn;
};