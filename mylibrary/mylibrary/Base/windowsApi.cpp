#include <imgui_impl_win32.h>

#include "windowsApi.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND windowsApi::hwnd;

LRESULT windowsApi::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return 1;
	}

	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY://ウインドウが破棄された
		PostQuitMessage(0);//OSに対して、アプリンの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//標準の処理を行う
}

windowsApi::~windowsApi()
{
	//ウインドウクラスを登録解除
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

void windowsApi::Initialize()
{
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WindowProc;//ウインドウプロシージャを設定
	wndClass.lpszClassName = L"DirectXGame";//ウインドウクラス名
	wndClass.hInstance = GetModuleHandle(nullptr);//ウインドウハンドル
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソル指定

	//ウインドウクラスをOSに登録
	RegisterClassEx(&wndClass);
	//ウインドウサイズ{X座標Y座標横幅縦幅}
	RECT wrc = { 0,0, window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//自動でサイズ補正

	//ウインドウオブジェクトの生成
	    hwnd = CreateWindow(wndClass.lpszClassName,//クラス名
		L"DirectXGame",                  //タイトルバーの文字
		WS_OVERLAPPEDWINDOW,             //標準的なウインドウスタイル
		CW_USEDEFAULT,                   //表示X座標(OSに任せる)
		CW_USEDEFAULT,                   //表示Y座標(OSに任せる)
		wrc.right - wrc.left,            //ウインドウ横幅
		wrc.bottom - wrc.top,            //ウインドウ縦幅
		nullptr,                         //親ウインドウハンドル
		nullptr,                         //メニューハンドル
		wndClass.hInstance,                     //呼び出しアプリケーションハンドル
		nullptr);                        //オプション

	//ウインドウ表示
	ShowWindow(hwnd, SW_SHOW);
}

bool windowsApi::Update()
{
	//メッセージがある？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);//キー入力メッセージの処理
		DispatchMessage(&msg);//プロジージャにメッセージを送る
	}

	//終了メッセージが来たらループを抜ける
	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

