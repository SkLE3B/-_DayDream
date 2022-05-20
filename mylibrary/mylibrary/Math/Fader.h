#pragma once
#include<d3d12.h>
#include<wrl.h>
#include<DirectxMath.h>
#include <string>
#include "../Base/Timer.h"

class Fader
{
private://�G�C���A�X
	using string = std::string;
public:
	/// <summary>
    /// �͈͒l
    /// </summary>
    /// <param name="max">�ő�l</param>
    /// <param name="min">�ŏ��l</param>
    /// <param name="pre">���ݒl</param>
	Fader(float max = 0, float min = 0, float pre = 0)
		:maxValue(max), minValue(min), preValue(pre) {}

	/// <summary>
    /// �t�F�[�h�C��
    /// </summary>
	void Fadein(float fadeValue);

	/// <summary>
	/// �t�F�[�h�A�E�g
	/// </summary>
	void Fadeout(float fadeValue);

	//���ݒlGetter
	float get() { return preValue; }

	float getMax() { return maxValue; }//�ő�lGetter
	float getMin() { return minValue; }//�ŏ��lGetter

	// ���ݒl���Z�b�g���鎞�͍ŏ��l�ȏ�A�ő�l�ȉ��ɂȂ�悤�ɂ���B
	void set(float value);

	// ���ݒl��ǉ��A�߂�l�ōŏI�I�Ȓl��Ԃ�
	float valueAdd(float value)
	{
		set(get() + value);
		return get();
	}

	//���ݒl�������A�߂�l�ōŏI�I�Ȓl��Ԃ�
	float valueMin(float value)
	{
		set(get() - value);
		return get();
	}

	//�l���ő�l���ǂ���
	bool isMax() {
		return (getMax() <= get()); // �ő�l�ȏ�Ȃ疞�^��
	}

	//�ȉ��Ȃ�
	bool Lessthan(const float value) {
		return  get() <= value;
	}

	//�ȏ�Ȃ�
	bool Morethan(const float value) {
		return  get() >= value;
	}

private:
	Timer time;
	float maxValue;//�ő�l
	float minValue;//�ŏ��l
	float preValue;//���ݒl
};