#pragma once
#include "BaseScene.h"
#include <DirectXMath.h>

class TitleScene :
    public BaseScene
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	static const int texNumber = 0;

public:
	TitleScene();

	~TitleScene();

	void Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
		DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post) override;

	void Update() override;

	void Draw() override;

	void UIDraw() override;

	void BgDraw() override;

	void FogDraw() override;

private:
	std::unique_ptr<Sprite> title;
	int time;
	bool fadeOutFlag;
	int time2;
};

