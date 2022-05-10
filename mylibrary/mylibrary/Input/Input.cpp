#include "Input.h"
#include <cassert>

using namespace std;

//�����o�֐��̒�`
bool Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;
	
	assert(!dinput);

	// DirectInput�I�u�W�F�N�g�̐���	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �L�[�{�[�h�f�o�C�X�̐���	
	result = dinput->CreateDevice(GUID_SysKeyboard, &devKeyboard, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �}�E�X�f�o�C�X�̐���	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// ���̓f�[�^�`���̃Z�b�g
	result = devKeyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �r�����䃌�x���̃Z�b�g
	result = devKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// ���̓f�[�^�`���̃Z�b�g
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // �W���`��
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �r�����䃌�x���̃Z�b�g
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

	{//�L�[�{�[�h
		//�L�[�{�[�h���̎擾�J�n
		result = devKeyboard->Acquire();

		// �O��̃L�[���͂�ۑ�
		memcpy(keyPre, key, sizeof(key));

		//�S�L�[�̓��͏�Ԃ��擾����
		result = devKeyboard->GetDeviceState(sizeof(key), key);
	}
	
	{
		// �}�E�X
		result = devMouse->Acquire();	// �}�E�X����J�n

		// �O��̓��͂�ۑ�
		mouseStatePre = mouseState;

		// �}�E�X�̓���
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
	// �ُ�Ȉ��������o
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber])
	{
		return true;
	}

	return false;
}

bool Input::RelesePush(int keyNumber)
{
	// �ُ�Ȉ��������o
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

	//�O�񉟂���Ă��Ȃ��A���A���񉟂���Ă���
	if (!keyPre[keyNumber] && key[keyNumber])
	{
		return true;
	}

	return false;
}

bool Input::PushMouseLeft()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::PushMouseMiddle()
{
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// �����Ă��Ȃ�
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
