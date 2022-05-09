#pragma once

#include<windows.h>

class windowsApi//ウィンドウAPI
{
public:
	//ウインドウサイズ
    static const int window_width = 1280;//横幅
	static const int window_height = 720;//縦幅
public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://メンバ関数
	///<summary>
	///初期化
	///</summary>
	void Initialize();

	///<summary>
	///初期化
	///</summary>
	bool Update();

	///<summary>
	///デストラクタ
	///</summary>
	~windowsApi();
	
	///<summary>
	///アクセッサ
	///</summary>
	static HWND& GetHwnd() {return hwnd;}

	const WNDCLASSEX& GetWndClass(){ return wndClass; }
private://メンバ変数
	//Window関連
	static HWND hwnd;//ウィンドウハンドル
	WNDCLASSEX wndClass{};//ウインドウクラスの設定

	MSG msg{};//メッセージ
};

