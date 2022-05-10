#pragma once

#include<d3d12.h>
#include<string>
#include<windows.h>

#include "Sprite.h"

//デバッグ文字列クラスの定義
class DebugText
{
public://定数
	static const int maxCharCount = 256;//最大文字数
	static const int fontWidth = 9;  //フォント画像内１文字文の横幅
	static const int fontHeight = 18; //フォント画像内1文字分の縦幅
	static const int fontLineCount = 14; //フォント
	static const int bufferSize = 256;	// 書式付き文字列展開用バッファサイズ

public:// 静的メンバ関数
	static DebugText* GetInstance();

public://メンバ関数
	
	~DebugText();

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float scale);
	void DrawAll(ID3D12GraphicsCommandList* cmdList);

private://メンバ変数
	//スプライトデータの配列
	Sprite* sprites[maxCharCount];
	//スプライトデータ配列の添え字番号
	int spriteIndex = 0;
};