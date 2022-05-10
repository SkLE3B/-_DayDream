#pragma once

#include<windows.h>

class windowsApi//�E�B���h�EAPI
{
public:
	//�E�C���h�E�T�C�Y
    static const int window_width = 1280;//����
	static const int window_height = 720;//�c��
public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://�����o�֐�
	///<summary>
	///������
	///</summary>
	void Initialize();

	///<summary>
	///������
	///</summary>
	bool Update();

	///<summary>
	///�f�X�g���N�^
	///</summary>
	~windowsApi();
	
	///<summary>
	///�A�N�Z�b�T
	///</summary>
	static HWND& GetHwnd() {return hwnd;}

	const WNDCLASSEX& GetWndClass(){ return wndClass; }
private://�����o�ϐ�
	//Window�֘A
	static HWND hwnd;//�E�B���h�E�n���h��
	WNDCLASSEX wndClass{};//�E�C���h�E�N���X�̐ݒ�

	MSG msg{};//���b�Z�[�W
};

