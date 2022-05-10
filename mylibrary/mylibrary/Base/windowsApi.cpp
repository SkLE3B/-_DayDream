#include <imgui_impl_win32.h>

#include "windowsApi.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND windowsApi::hwnd;

LRESULT windowsApi::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return 1;
	}

	//���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY://�E�C���h�E���j�����ꂽ
		PostQuitMessage(0);//OS�ɑ΂��āA�A�v�����̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�W���̏������s��
}

windowsApi::~windowsApi()
{
	//�E�C���h�E�N���X��o�^����
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

void windowsApi::Initialize()
{
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WindowProc;//�E�C���h�E�v���V�[�W����ݒ�
	wndClass.lpszClassName = L"DirectXGame";//�E�C���h�E�N���X��
	wndClass.hInstance = GetModuleHandle(nullptr);//�E�C���h�E�n���h��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//�J�[�\���w��

	//�E�C���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&wndClass);
	//�E�C���h�E�T�C�Y{X���WY���W�����c��}
	RECT wrc = { 0,0, window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�����ŃT�C�Y�␳

	//�E�C���h�E�I�u�W�F�N�g�̐���
	    hwnd = CreateWindow(wndClass.lpszClassName,//�N���X��
		L"DirectXGame",                  //�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,             //�W���I�ȃE�C���h�E�X�^�C��
		CW_USEDEFAULT,                   //�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,                   //�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,            //�E�C���h�E����
		wrc.bottom - wrc.top,            //�E�C���h�E�c��
		nullptr,                         //�e�E�C���h�E�n���h��
		nullptr,                         //���j���[�n���h��
		wndClass.hInstance,                     //�Ăяo���A�v���P�[�V�����n���h��
		nullptr);                        //�I�v�V����

	//�E�C���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}

bool windowsApi::Update()
{
	//���b�Z�[�W������H
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);//�L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&msg);//�v���W�[�W���Ƀ��b�Z�[�W�𑗂�
	}

	//�I�����b�Z�[�W�������烋�[�v�𔲂���
	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

