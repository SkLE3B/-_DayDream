#include "Gamepad.h"
#include "Base/windowsApi.h"

DeviceEnumParameter Gamepad::parameter;
DIJOYSTATE Gamepad::pad_data;
DIJOYSTATE Gamepad::pre_pad_data;
const float Gamepad::MAX = 1000.0f;

Gamepad::Gamepad()
{
}

Gamepad::~Gamepad()
{
}

//デバイス列挙するコールバック関数
BOOL CALLBACK GetJoystickCallback(LPDIDEVICEINSTANCE lpddi, LPVOID /*pvRef*/)
{
	HRESULT result = Gamepad::parameter.dinput->CreateDevice(lpddi->guidInstance, &Gamepad::parameter.GamePadDevice, NULL);
	if (result == E_FAIL)	//if(FAILED(result))
	{
		return DIENUM_CONTINUE;	// 次のデバイスを列挙
	}
	return DIENUM_STOP;	// デバイスの列挙を中止
}

void Gamepad::Initialize()
{
	HRESULT result;
	ComPtr<IDirectInput8> dinput = nullptr;
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	parameter.FindCount = 0;
	parameter.GamePadDevice = dInputDevice;
	result = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dinput,
		nullptr);
	parameter.dinput = dinput;
	// ジョイパッドを探す
	dinput->EnumDevices(DI8DEVCLASS_GAMECTRL,
		(LPDIENUMDEVICESCALLBACK)GetJoystickCallback, nullptr, DIEDFL_ATTACHEDONLY);
	dInputDevice = parameter.GamePadDevice;
	if (dInputDevice == nullptr)
	{
		OutputDebugStringA("ゲームパッドが見つかりませんでした\n");
		return;
	}
	// ジョイスティック用のデータ・フォーマットを設定
	result = dInputDevice->SetDataFormat(&c_dfDIJoystick);
	// モードを設定（フォアグラウンド＆非排他モード）
	result = dInputDevice->SetCooperativeLevel(windowsApi::GetHwnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	// 軸の値の範囲を設定
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = (int)-MAX;	//最小値
	diprg.lMax = (int)MAX;	//最大値
	dInputDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	diprg.diph.dwObj = DIJOFS_Y;
	dInputDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	diprg.diph.dwObj = DIJOFS_Z;
	dInputDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	diprg.diph.dwObj = DIJOFS_RZ;
	dInputDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
}

void Gamepad::Update()
{
	HRESULT result;

	if (dInputDevice == nullptr)
	{
		return;
	}

	//ゲームパッドの入力情報取得
	dInputDevice->Acquire();
	result = dInputDevice->Poll();
	pre_pad_data = pad_data;
	result = dInputDevice->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
}

//押している間
bool Gamepad::IsKeyDown(int number)
{
	if (number < 0 || number > 32)
	{
		return false;
	}
	if (pad_data.rgbButtons[number] >= 0x80)
	{
		return true;
	}
	return false;
}

//押した瞬間
bool Gamepad::IsKeyPush(int number)
{
	if (number < 0 || number > 32)
	{
		return false;
	}
	if (pad_data.rgbButtons[number] >= 0x80 && pre_pad_data.rgbButtons[number] <= 0x00)
	{
		return true;
	}
	return false;
}

//離している間
bool Gamepad::IsKeyUp(int number)
{
	if (number < 0 || number > 32)
	{
		return false;
	}
	if (pad_data.rgbButtons[number] <= 0x00 && pre_pad_data.rgbButtons[number] >= 0x80)
	{
		return true;
	}
	return false;
}

//離した瞬間
bool Gamepad::IsKeyRelease(int number)
{
	if (number < 0 || number > 32)
	{
		return false;
	}
	if (pad_data.rgbButtons[number] <= 0x00 && pre_pad_data.rgbButtons[number] >= 0x80)
	{
		return true;
	}

	return false;
}