#pragma once

#define DIRECTINPUT_VERSION   0x0800//// DirectInput �̃o�[�W�����w��
#include <dinput.h>

#include<windows.h>
#include <wrl.h>

/// <summary>
/// ����
/// </summary>
class Input
{
public:
	struct MouseMove {
		LONG    lX;
		LONG    lY;
		LONG    lZ;
	};
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // �ÓI�����o�ϐ�
	static Input* GetInstance();
public:
	//�����o�֐��̐錾
	//������
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	
	//���t���[������
	void Update();
	//�L�[��������Ă��邩�`�F�b�N
	bool IsPush(int keyNumber);

	//�L�[��������Ă��Ȃ����`�F�b�N
	bool RelesePush(int keyNumber);

	//�L�[�̃g���K�[�`�F�b�N
	bool TriggerPush(int keyNumber);

	/// <summary>
	/// �L�[�̍��{�^���������`�F�b�N
	/// </summary>
	/// <returns>������Ă��邩</returns>
	bool PushMouseLeft();

	/// <summary>
	/// �L�[�̒��{�^���������`�F�b�N
	/// </summary>
	/// <returns>������Ă��邩</returns>
	bool PushMouseMiddle();

	/// <summary>
	/// �}�E�X�ړ��ʂ��擾
	/// </summary>
	/// <returns>�}�E�X�ړ���</returns>
	MouseMove GetMouseMove();

private://�����o�ϐ�
	ComPtr<IDirectInput8> dinput;
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> devKeyboard;
	//�L�[���͏��
	BYTE key[256] = {};
	//�O��̃L�[���͏��
	BYTE keyPre[256] = {};
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};

