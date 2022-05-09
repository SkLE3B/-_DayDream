#pragma once

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#pragma comment(lib,"dinput8.lib")

using namespace Microsoft::WRL;

//ゲームパッドデバイスの作成-デバイス列挙の結果を受け取る構造体
struct DeviceEnumParameter
{
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> GamePadDevice;
	int FindCount;
};

//DirectXInputのコード
enum GAMEPAD_BUTTON
{
	GPB_X,		//Xボタン
	GPB_Y,		//Yボタン
	GPB_A,		//Aボタン
	GPB_B,		//Bボタン
	GPB_LB,		//Lボタン
	GPB_RB,		//Rボタン
	GPB_LT,		//Lトリガー
	GPB_RT,		//Rトリガー
	GPB_LS,		//Lスティック押し込み
	GPB_RS,		//Rスティック押し込み
	GPB_BACK,
	GPB_START,
	GPB_GUIDE,
};

class Gamepad
{
private:
	ComPtr<IDirectInputDevice8> dInputDevice = nullptr;
	static const float MAX;
	static DIJOYSTATE pad_data;
	static DIJOYSTATE pre_pad_data;
public:
	Gamepad();
	~Gamepad();
	static DeviceEnumParameter parameter;
	void Initialize();
	void Update();
	static bool IsKeyDown(int number);
	static bool IsKeyPush(int number);
	static bool IsKeyUp(int number);
	static bool IsKeyRelease(int number);
	//Lアナログスティック
	static float angleLX() { return (float)pad_data.lX / MAX; }
	static float angleLY() { return (float)pad_data.lY / -MAX; }
	//Rアナログスティック
	static float angleRX() { return (float)pad_data.lZ / MAX; }
	static float angleRY() { return (float)pad_data.lRz / -MAX; }
};