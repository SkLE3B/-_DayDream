#pragma once
#include<d3d12.h>
#include<wrl.h>
#include<DirectxMath.h>
#include <string>
#include "../Base/Timer.h"

class Fader
{
private://エイリアス
	using string = std::string;
public:
	/// <summary>
    /// 範囲値
    /// </summary>
    /// <param name="max">最大値</param>
    /// <param name="min">最小値</param>
    /// <param name="pre">現在値</param>
	Fader(float max = 0, float min = 0, float pre = 0)
		:maxValue(max), minValue(min), preValue(pre) {}

	/// <summary>
    /// フェードイン
    /// </summary>
	void Fadein(float fadeValue);

	/// <summary>
	/// フェードアウト
	/// </summary>
	void Fadeout(float fadeValue);

	//現在値Getter
	float get() { return preValue; }

	float getMax() { return maxValue; }//最大値Getter
	float getMin() { return minValue; }//最小値Getter

	// 現在値をセットする時は最小値以上、最大値以下になるようにする。
	void set(float value);

	// 現在値を追加、戻り値で最終的な値を返す
	float valueAdd(float value)
	{
		set(get() + value);
		return get();
	}

	//現在値を減少、戻り値で最終的な値を返す
	float valueMin(float value)
	{
		set(get() - value);
		return get();
	}

	//値が最大値かどうか
	bool isMax() {
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

private:
	Timer time;
	float maxValue;//最大値
	float minValue;//最小値
	float preValue;//現在値
};