#pragma once
#include <algorithm>

class ExNum
{
private:
	float mMax;//最大値
	float mMin;//最小値
	float mPre;//現在値

public:
	/// <summary>
	/// 範囲値
	/// </summary>
	/// <param name="max">最大値</param>
	/// <param name="min">最小値</param>
	/// <param name="pre">現在値</param>
	ExNum(float max = 0,float min = 0,float pre = 0)
		:mMax(max),mMin(min),mPre(pre){}

	float getMax() { return mMax; }//最大値Getter
	float getMin() { return mMin; }//最小値Getter
	float get() { return mPre; }   //現在値Getter

	// 最大値をセットする時は最小値より小さくならないようにする。
    // また現在値が最大値を超えているなら、最大値と同じにする
	void setMax(float value);

	// 最小値をセットする時は最大値より大きくならないようにする。
	// あと、現在値が最小値未満になるなら、最小値と同じにする。
	void setMin(float value);

	// 現在値をセットする時は最小値以上、最大値以下になるようにする。
	void set(float value);

	// 現在値を追加、戻り値で最終的な値を返す
	float add(float value)
	{
		set(get() + value);
		return get();
	}

	// 最大値を追加
	float addMax(float value)
	{
		setMax(getMax() + value);
		return getMax();
	}

	// 最小値を追加
	float addMin(float value) {
		setMin(getMin() + value);
		return getMin();
	}
	
	// HPが空です。
	bool isEmpty() {
		return (get() <= getMin()); // 最小値以下なら空
	}

	// HPが満タンです
	bool isFull() {
		return (getMax() <= get()); // 最大値以上なら満タン
	}

	//以下なら
	bool Lessthan(const float value) {
		return  get() <= value;
	}

	//以上なら
	bool Morethan(const float value) {
		return  get() >= value;
	}

	// 割合を取得
	float rate() {
		return (get() / getMax()); // 現在値 / 最大値
	}
};

