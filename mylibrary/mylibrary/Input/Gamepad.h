#pragma once

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#pragma comment(lib,"dinput8.lib")

using namespace Microsoft::WRL;

//�Q�[���p�b�h�f�o�C�X�̍쐬-�f�o�C�X�񋓂̌��ʂ��󂯎��\����
struct DeviceEnumParameter
{
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> GamePadDevice;
	int FindCount;
};

//DirectXInput�̃R�[�h
enum GAMEPAD_BUTTON
{
	GPB_X,		//X�{�^��
	GPB_Y,		//Y�{�^��
	GPB_A,		//A�{�^��
	GPB_B,		//B�{�^��
	GPB_LB,		//L�{�^��
	GPB_RB,		//R�{�^��
	GPB_LT,		//L�g���K�[
	GPB_RT,		//R�g���K�[
	GPB_LS,		//L�X�e�B�b�N��������
	GPB_RS,		//R�X�e�B�b�N��������
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
	//L�A�i���O�X�e�B�b�N
	static float angleLX() { return (float)pad_data.lX / MAX; }
	static float angleLY() { return (float)pad_data.lY / -MAX; }
	//R�A�i���O�X�e�B�b�N
	static float angleRX() { return (float)pad_data.lZ / MAX; }
	static float angleRY() { return (float)pad_data.lRz / -MAX; }
};