#include "EndScene.h"
#include "TitleScene.h"

EndScene::EndScene()
{
	scenePattern = Scene::End;
}

EndScene::~EndScene()
{
}

void EndScene::Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng, DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText, Fog* fog, PostEffect* post)
{
	BaseScene::Initialize(dxCommon, input, audio, tMng, dCamera, bCamera, gamepad, dText,fog,post);
	sceneChengeFlag = false;

	audio->Stop();

	//サウンドの読み込み
	audio->PlayLoop(L"Resources/sounds/End.wav");
	//テクスチャ読み込み
	tMng->spriteLoadTexture(7, L"Resources/textures/End.png");

	post->ResetFade(1.0f, post->GetFadeOutFlag());

	end = std::make_unique<Sprite>();
	end->Initialize(7);
	end->SetSize(1280, 720);
	time = 0;
	fadeOutFlag = false;
}

void EndScene::Update()
{
	// Enterで指定のシーンへ
	if (input->TriggerPush(DIK_SPACE) && !fadeOutFlag)
	{
		audio->PlayWave(L"Resources/sounds/button06 .wav");
		fadeOutFlag = !fadeOutFlag;
	}

	if (fadeOutFlag)
	{
		time++;
	}

	if (time >= 10)
	{
		end->ChengeFadeOutFlag();
		ChangeSceneChengeFlag();
		fog->FadeOut(0.01f);
	}

	//370
	if (time >= 210)
	{
		fadeOutFlag = !fadeOutFlag;
		time = 0;
		//タイトルシーンへ
		SceneManager::GetInstance()->ChangeScene("TITLE");
		SceneManager::GetInstance()->SetSceneState(Scene::Title);
	}
}

void EndScene::Draw()
{
}

void EndScene::UIDraw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//スプライトのパイプラインをセット
	Sprite::SetPipelineState(dxCommon->GetCommandList());
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	end->Draw(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
}

void EndScene::BgDraw()
{
}

void EndScene::FogDraw()
{
}
