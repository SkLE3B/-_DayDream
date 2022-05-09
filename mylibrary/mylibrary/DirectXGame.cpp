#include "DirectXGame.h"
#include<DirectXMath.h>
#include<windows.h>
#include<DirectXMath.h>
#include<d3dcompiler.h>
#include<string>
#include<DirectXTex.h>
#include<wrl.h>
#include "3d/Object3d.h"
#include "Collision/SphereCollider.h"
#include "Collision/MeshCollider.h"
#include "Collision/CollisionManager.h"
#include "Player.h"
#include "3d/TouchableObject.h"
#include "Scene/AbstractSceneFactory.h"
#include "Scene/SceneFactory.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

DirectXGame* DirectXGame::GetInstance()
{
	static DirectXGame instance;

	return &instance;
}

void DirectXGame::Initialize()
{
	//基底クラスの初期化
	GameBase::Initialize();

#pragma region シーン初期化
	// シーンファクトリーを生成し、マネージャーをセット
	sceneFactory = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory);
	SceneManager::GetInstance()->ChangeScene("TITLE");
#pragma endregion
}

void DirectXGame::Update()
{
	//基底クラスの更新
	GameBase::Update();

	//シーン更新
	SceneManager::GetInstance()->Update(dxBase.get(), input, audioManger.get(), textureManager.get(), Dcamera.get(), Bcamera.get(), gamepad.get(),
		debugText->GetInstance(),testfog.get(),postEffect);

	EffekseerManager::Update(dxBase.get(), Dcamera.get());
}

void DirectXGame::Draw()
{
	if (SceneManager::GetInstance()->GetSceneState() == Scene::Title)
	{
		testfog->PreDrawScene(dxBase->GetCommandList());
		SceneManager::GetInstance()->Draw();
		SceneManager::GetInstance()->FogDraw();
		testfog->PostDrawScene(dxBase->GetCommandList());

		//if (SceneManager::GetInstance()->GetSceneChengeFlag())
		//{
		//	testfog->ChengeFadeOutFlag();
		//}
	}

	if (SceneManager::GetInstance()->GetSceneState() == Scene::Game)
	{
		testfog->PreDrawScene(dxBase->GetCommandList());
		SceneManager::GetInstance()->Draw();
		SceneManager::GetInstance()->FogDraw();
		testfog->PostDrawScene(dxBase->GetCommandList());

		if (SceneManager::GetInstance()->GetSceneChengeFlag())
		{
			testfog->ChengeFadeOutFlag();
		}		
	}
	
	if (SceneManager::GetInstance()->GetSceneState() == Scene::End)
	{
		postEffect->PreDrawScene(dxBase->GetCommandList());
		SceneManager::GetInstance()->FogDraw();
		SceneManager::GetInstance()->Draw();
		SceneManager::GetInstance()->UIDraw();
		postEffect->PostDrawScene(dxBase->GetCommandList());

		if (SceneManager::GetInstance()->GetSceneChengeFlag())
		{
			postEffect->ChengeFadeOutFlag();
		}
	}
		
	//描画開始
	dxBase->BeginDraw();
	//背面スプライト描画
	//ポストエフェクト;
	SceneManager::GetInstance()->BgDraw();
	if (SceneManager::GetInstance()->GetSceneState() == Scene::Title)
	{
		testfog->Draw(dxBase->GetCommandList());
	}

	if (SceneManager::GetInstance()->GetSceneState() == Scene::Game)
	{
		testfog->Draw(dxBase->GetCommandList());
	}
	
	if (SceneManager::GetInstance()->GetSceneState() == Scene::End)
	{
		testfog->Draw(dxBase->GetCommandList());
	}
	
	SceneManager::GetInstance()->UIDraw();
	EffekseerManager::Draw();
	dxBase->EndDraw();
	//ここまで描画コマンド
	flamerate->Wait();
}

void DirectXGame::Finalize()
{
	//基底クラスの後始末
	GameBase::Finalize();
}

