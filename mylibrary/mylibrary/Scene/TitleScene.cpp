#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"

TitleScene::TitleScene()
{
	scenePattern = Scene::Title;
}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post)
{
	BaseScene::Initialize(dxCommon, input, audio, tMng, dCamera, bCamera, gamepad, dText,fog,post);
	sceneChengeFlag = false;

	audio->Stop();

	//サウンドの読み込み
	audio->PlayLoop(L"Resources/sounds/宇宙を彩る銀ペンキ.wav");
	//テクスチャ読み込み
	tMng->spriteLoadTexture(6, L"Resources/textures/Title.png");

	title = std::make_unique<Sprite>();
	title->Initialize(6);
	title->SetSize(1280, 720);
	title->SetPosition(0,-20);
	time = 0;
	time2 = 0;
	fadeOutFlag = false;
}			

void TitleScene::Update()
{
	// Enterで指定のシーンへ
	if (input->TriggerPush(DIK_SPACE) && !fadeOutFlag)
	{
		audio->PlayWave(L"Resources/sounds/button06 .wav");
		fadeOutFlag = !fadeOutFlag;
		title->ChengeFadeOutFlag();
	}

	time2++;

	if (time2 >= 60)
	{
		time2 = 0;
	}
	
	if (fadeOutFlag)
	{
		time++;
	}

	if (time >= 200)
	{
		fadeOutFlag = !fadeOutFlag;
		time = 0;
		//ゲームシーンへ
		SceneManager::GetInstance()->ChangeScene("GAME");
		SceneManager::GetInstance()->SetSceneState(Scene::Game);
	}
}

void TitleScene::Draw()
{
}

void TitleScene::UIDraw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//スプライトのパイプラインをセット
	Sprite::SetPipelineState(dxCommon->GetCommandList());
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	title->Draw(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
}

void TitleScene::BgDraw()
{
}

void TitleScene::FogDraw()
{
}
