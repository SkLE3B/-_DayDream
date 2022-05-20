#pragma once
#include <algorithm>

class ExNum
{
private:
	float mMax;//�ő�l
	float mMin;//�ŏ��l
	float mPre;//���ݒl

public:
	/// <summary>
	/// �͈͒l
	/// </summary>
	/// <param name="max">�ő�l</param>
	/// <param name="min">�ŏ��l</param>
	/// <param name="pre">���ݒl</param>
	ExNum(float max = 0,float min = 0,float pre = 0)
		:mMax(max),mMin(min),mPre(pre){}

	float getMax() { return mMax; }//�ő�lGetter
	float getMin() { return mMin; }//�ŏ��lGetter
	float get() { return mPre; }   //���ݒlGetter

	// �ő�l���Z�b�g���鎞�͍ŏ��l��菬�����Ȃ�Ȃ��悤�ɂ���B
    // �܂����ݒl���ő�l�𒴂��Ă���Ȃ�A�ő�l�Ɠ����ɂ���
	void setMax(float value);

	// �ŏ��l���Z�b�g���鎞�͍ő�l���傫���Ȃ�Ȃ��悤�ɂ���B
	// ���ƁA���ݒl���ŏ��l�����ɂȂ�Ȃ�A�ŏ��l�Ɠ����ɂ���B
	void setMin(float value);

	// ���ݒl���Z�b�g���鎞�͍ŏ��l�ȏ�A�ő�l�ȉ��ɂȂ�悤�ɂ���B
	void set(float value);

	// ���ݒl��ǉ��A�߂�l�ōŏI�I�Ȓl��Ԃ�
	float add(float value)
	{
		set(get() + value);
		return get();
	}

	// �ő�l��ǉ�
	float addMax(float value)
	{
		setMax(getMax() + value);
		return getMax();
	}

	// �ŏ��l��ǉ�
	float addMin(float value) {
		setMin(getMin() + value);
		return getMin();
	}
	
	// HP����ł��B
	bool isEmpty() {
		return (get() <= getMin()); // �ŏ��l�ȉ��Ȃ��
	}

	// HP�����^���ł�
	bool isFull() {
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

	// �������擾
	float rate() {
		return (get() / getMax()); // ���ݒl / �ő�l
	}
};

