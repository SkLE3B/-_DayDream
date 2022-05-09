#include "DebugText.h"

DebugText* DebugText::GetInstance()
{
	static DebugText instance;
	return &instance;
}

DebugText::~DebugText()
{
	//全てのスプライトデータについて
	for (int i = 0; i < _countof(sprites); i++)
	{
		//スプライトを解放する
		delete sprites[i];
		sprites[i] = nullptr;
	}
}

void DebugText::Initialize(UINT texnumber)
{
	//全てのスプライトデータについて
	for (int i = 0; i < _countof(sprites); i++)
	{
		//スプライトを生成する
		sprites[i] = new Sprite();
		//スプライトを初期化する
		sprites[i]->Initialize(texnumber);
	}
}

//1文字列追加
void DebugText::Print(const std::string& text, float x, float y, float scale = 1.0f)
{
	//全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数超過
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		//1文字取り出す
		const unsigned char& character = text[i];

		//ASCIIコードの2段文飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex]->SetPosition(x + fontWidth * scale * i, y);
		sprites[spriteIndex]->SetTextureRect((float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight, fontWidth, fontHeight);
		sprites[spriteIndex]->SetSize(fontWidth * scale, fontHeight * scale);

		//文字列を1つ進める
		spriteIndex++;
	}
}

void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
	//全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		sprites[i]->Draw(cmdList);
	}

	spriteIndex = 0;
}