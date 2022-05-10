#pragma once
#include "BaseScene.h"
#include <DirectXMath.h>

class TitleScene :
    public BaseScene
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // �ÓI�����o�ϐ�
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

