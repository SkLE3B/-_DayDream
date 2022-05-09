#include "Input.h"
#include <cassert>

using namespace std;

//メンバ関数の定義
bool Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;
	
	assert(!dinput);

	// DirectInputオブジェクトの生成	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// キーボードデバイスの生成	
	result = dinput->CreateDevice(GUID_SysKeyboard, &devKeyboard, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 入力データ形式のセット
	result = devKeyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 排他制御レベルのセット
	result = devKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 入力データ形式のセット
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 排他制御レベルのセット
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

void Input::Update()
{
	HRESULT result;

	{//キーボード
		//キーボード情報の取得開始
		result = devKeyboard->Acquire();

		// 前回のキー入力を保存
		memcpy(keyPre, key, sizeof(key));

		//全キーの入力状態を取得する
		result = devKeyboard->GetDeviceState(sizeof(key), key);
	}
	
	{
		// マウス
		result = devMouse->Acquire();	// マウス動作開始

		// 前回の入力を保存
		mouseStatePre = mouseState;

		// マウスの入力
		result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}	
}

Input * Input::GetInstance()
{
	static Input instance;

	return &instance;
}

bool Input::IsPush(int keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber])
	{
		return true;
	}

	return false;
}

bool Input::RelesePush(int keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	if (keyPre[keyNumber] && !key[keyNumber])
	{
		return true;
	}

	return false;
}

bool Input::TriggerPush(int keyNumber)
{

	if (keyNumber < 0x00) return false;
	if (keyNumber > 0xff) return false;

	//前回押されていない、かつ、今回押されている
	if (!keyPre[keyNumber] && key[keyNumber])
	{
		return true;
	}

	return false;
}

bool Input::PushMouseLeft()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseMiddle()
{
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// 押していない
	return false;
}

Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState.lX;
	tmp.lY = mouseState.lY;
	tmp.lZ = mouseState.lZ;
	return tmp;
}
