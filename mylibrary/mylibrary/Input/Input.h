#pragma once

#define DIRECTINPUT_VERSION   0x0800//// DirectInput のバージョン指定
#include <dinput.h>

#include<windows.h>
#include <wrl.h>

/// <summary>
/// 入力
/// </summary>
class Input
{
public:
	struct MouseMove {
		LONG    lX;
		LONG    lY;
		LONG    lZ;
	};
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // 静的メンバ変数
	static Input* GetInstance();
public:
	//メンバ関数の宣言
	//初期化
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	
	//毎フレーム処理
	void Update();
	//キーがおされているかチェック
	bool IsPush(int keyNumber);

	//キーが押されていないかチェック
	bool RelesePush(int keyNumber);

	//キーのトリガーチェック
	bool TriggerPush(int keyNumber);

	/// <summary>
	/// キーの左ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	bool PushMouseLeft();

	/// <summary>
	/// キーの中ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	bool PushMouseMiddle();

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	MouseMove GetMouseMove();

private://メンバ変数
	ComPtr<IDirectInput8> dinput;
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> devKeyboard;
	//キー入力情報
	BYTE key[256] = {};
	//前回のキー入力情報
	BYTE keyPre[256] = {};
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};

