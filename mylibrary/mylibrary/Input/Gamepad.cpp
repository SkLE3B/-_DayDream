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

//�f�o�C�X�񋓂���R�[���o�b�N�֐�
BOOL CALLBACK GetJoystickCallback(LPDIDEVICEINSTANCE lpddi, LPVOID /*pvRef*/)
{
	HRESULT result = Gamepad::parameter.dinput->CreateDevice(lpddi->guidInstance, &Gamepad::parameter.GamePadDevice, NULL);
	if (result == E_FAIL)	//if(FAILED(result))
	{
		return DIENUM_CONTINUE;	// ���̃f�o�C�X���
	}
	return DIENUM_STOP;	// �f�o�C�X�̗񋓂𒆎~
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
	// �W���C�p�b�h��T��
	dinput->EnumDevices(DI8DEVCLASS_GAMECTRL,
		(LPDIENUMDEVICESCALLBACK)GetJoystickCallback, nullptr, DIEDFL_ATTACHEDONLY);
	dInputDevice = parameter.GamePadDevice;
	if (dInputDevice == nullptr)
	{
		OutputDebugStringA("�Q�[���p�b�h��������܂���ł���\n");
		return;
	}
	// �W���C�X�e�B�b�N�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
	result = dInputDevice->SetDataFormat(&c_dfDIJoystick);
	// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	result = dInputDevice->SetCooperativeLevel(windowsApi::GetHwnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	// ���̒l�͈̔͂�ݒ�
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = (int)-MAX;	//�ŏ��l
	diprg.lMax = (int)MAX;	//�ő�l
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

	//�Q�[���p�b�h�̓��͏��擾
	dInputDevice->Acquire();
	result = dInputDevice->Poll();
	pre_pad_data = pad_data;
	result = dInputDevice->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
}

//�����Ă����
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

//�������u��
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

//�����Ă����
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

//�������u��
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